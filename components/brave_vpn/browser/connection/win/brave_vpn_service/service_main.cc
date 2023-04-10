/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// This macro is used in <wrl/module.h>. Since only the COM functionality is
// used here (while WinRT isn't being used), define this macro to optimize
// compilation of <wrl/module.h> for COM-only.

#ifndef __WRL_CLASSIC_COM_STRICT__
#define __WRL_CLASSIC_COM_STRICT__
#endif  // __WRL_CLASSIC_COM_STRICT__

#include "brave/components/brave_vpn/browser/connection/win/brave_vpn_service/service_main.h"

#include <windows.h>
#include <utility>

#include <atlsecurity.h>
#include <sddl.h>
#include <wrl/module.h>
#include <type_traits>

#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/path_service.h"
#include "base/run_loop.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task/single_thread_task_executor.h"
#include "base/threading/thread_restrictions.h"
#include "base/win/scoped_com_initializer.h"
#include "brave/components/brave_vpn/browser/connection/win/brave_vpn_service/brave_vpn_service.h"
#include "brave/components/brave_vpn/browser/connection/win/brave_vpn_service/service_constants.h"
#include "brave/components/brave_vpn/browser/connection/win/brave_vpn_service/service_utils.h"

namespace brave_vpn {
namespace {
HRESULT HRESULTFromLastError() {
  const auto error_code = ::GetLastError();
  return (error_code != NO_ERROR) ? HRESULT_FROM_WIN32(error_code) : E_FAIL;
}

// Command line switch "--console" runs the service interactively for
// debugging purposes.
constexpr char kConsoleSwitchName[] = "console";
}  // namespace

ServiceMain* ServiceMain::GetInstance() {
  static base::NoDestructor<ServiceMain> instance;
  return instance.get();
}

bool ServiceMain::InitWithCommandLine(const base::CommandLine* command_line) {
  VLOG(1) << __func__ << ":" << command_line->GetCommandLineString();
  const base::CommandLine::StringVector args = command_line->GetArgs();
  if (!args.empty()) {
    LOG(ERROR) << "No positional parameters expected.";
    return false;
  }

  // Run interactively if needed.
  if (command_line->HasSwitch(kConsoleSwitchName)) {
    run_routine_ = &ServiceMain::RunInteractive;
  }
  return true;
}

// Start() is the entry point called by WinMain.
int ServiceMain::Start() {
  return (this->*run_routine_)();
}

void ServiceMain::CreateWRLModule() {
  Microsoft::WRL::Module<Microsoft::WRL::OutOfProc>::Create(
      this, &ServiceMain::SignalExit);
}

// When _ServiceMain gets called, it initializes COM, and then calls Run().
// Run() initializes security, then calls RegisterClassObject().
HRESULT ServiceMain::RegisterClassObject() {
  auto& module = Microsoft::WRL::Module<Microsoft::WRL::OutOfProc>::GetModule();

  // We hand-register a unique CLSID for each Chrome channel.
  Microsoft::WRL::ComPtr<IUnknown> factory;
  unsigned int flags = Microsoft::WRL::ModuleType::OutOfProc;

  HRESULT hr = Microsoft::WRL::Details::CreateClassFactory<
      Microsoft::WRL::SimpleClassFactory<BraveVpnService>>(
      &flags, nullptr, __uuidof(IClassFactory), &factory);
  if (FAILED(hr)) {
    LOG(ERROR) << "Factory creation failed; hr: " << hr;
    return hr;
  }

  Microsoft::WRL::ComPtr<IClassFactory> class_factory;
  hr = factory.As(&class_factory);
  if (FAILED(hr)) {
    LOG(ERROR) << "IClassFactory object creation failed; hr: " << hr;
    return hr;
  }

  // The pointer in this array is unowned. Do not release it.
  IClassFactory* class_factories[] = {class_factory.Get()};
  static_assert(std::extent<decltype(cookies_)>() == std::size(class_factories),
                "Arrays cookies_ and class_factories must be the same size.");

  IID class_ids[] = {GetBraveVpnServiceClsid()};
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kBraveVpnServiceClsIdForTestingSwitch)) {
    class_ids[0] = {kTestBraveVpnServiceClsid};
  }

  DCHECK_EQ(std::size(cookies_), std::size(class_ids));
  static_assert(std::extent<decltype(cookies_)>() == std::size(class_ids),
                "Arrays cookies_ and class_ids must be the same size.");

  hr = module.RegisterCOMObject(nullptr, class_ids, class_factories, cookies_,
                                std::size(cookies_));
  if (FAILED(hr)) {
    LOG(ERROR) << "RegisterCOMObject failed; hr: " << hr;
    return hr;
  }

  return hr;
}

void ServiceMain::UnregisterClassObject() {
  auto& module = Microsoft::WRL::Module<Microsoft::WRL::OutOfProc>::GetModule();
  const HRESULT hr =
      module.UnregisterCOMObject(nullptr, cookies_, std::size(cookies_));
  if (FAILED(hr)) {
    LOG(ERROR) << "UnregisterCOMObject failed; hr: " << hr;
  }
}

ServiceMain::ServiceMain()
    : run_routine_(&ServiceMain::RunAsService),
      service_status_handle_(nullptr),
      service_status_() {
  service_status_.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  service_status_.dwCurrentState = SERVICE_STOPPED;
  service_status_.dwControlsAccepted = SERVICE_ACCEPT_STOP;
}

ServiceMain::~ServiceMain() = default;

int ServiceMain::RunAsService() {
  VLOG(1) << __func__;
  const std::wstring& service_name(brave_vpn::GetBraveVpnServiceName());
  const SERVICE_TABLE_ENTRY dispatch_table[] = {
      {const_cast<LPTSTR>(service_name.c_str()),
       &ServiceMain::ServiceMainEntry},
      {nullptr, nullptr}};

  if (!::StartServiceCtrlDispatcher(dispatch_table)) {
    service_status_.dwWin32ExitCode = ::GetLastError();
    LOG(ERROR) << "Failed to connect to the service control manager:"
               << service_status_.dwWin32ExitCode;
  }

  return service_status_.dwWin32ExitCode;
}

void ServiceMain::ServiceMainImpl() {
  VLOG(1) << __func__ << " BraveVPN Service started";
  service_status_handle_ =
      ::RegisterServiceCtrlHandler(brave_vpn::GetBraveVpnServiceName().c_str(),
                                   &ServiceMain::ServiceControlHandler);
  if (service_status_handle_ == nullptr) {
    LOG(ERROR) << "RegisterServiceCtrlHandler failed";
    return;
  }
  SetServiceStatus(SERVICE_RUNNING);
  service_status_.dwWin32ExitCode = ERROR_SUCCESS;
  service_status_.dwCheckPoint = 0;
  service_status_.dwWaitHint = 0;

  // Initialize COM for the current thread.
  base::win::ScopedCOMInitializer com_initializer(
      base::win::ScopedCOMInitializer::kMTA);
  if (!com_initializer.Succeeded()) {
    PLOG(ERROR) << "Failed to initialize COM";
    SetServiceStatus(SERVICE_STOPPED);
    return;
  }
  // When the Run function returns, the service has stopped.
  const HRESULT hr = Run();
  if (FAILED(hr)) {
    service_status_.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
    service_status_.dwServiceSpecificExitCode = hr;
  }

  SetServiceStatus(SERVICE_STOPPED);
}

int ServiceMain::RunInteractive() {
  return Run();
}

// static
void ServiceMain::ServiceControlHandler(DWORD control) {
  ServiceMain* self = ServiceMain::GetInstance();
  switch (control) {
    case SERVICE_CONTROL_STOP:
      self->SignalExit();
      break;

    default:
      break;
  }
}

// static
void WINAPI ServiceMain::ServiceMainEntry(DWORD argc, wchar_t* argv[]) {
  ServiceMain::GetInstance()->ServiceMainImpl();
}

void ServiceMain::SetServiceStatus(DWORD state) {
  ::InterlockedExchange(&service_status_.dwCurrentState, state);
  ::SetServiceStatus(service_status_handle_, &service_status_);
}

HRESULT ServiceMain::CreateWGConnection() {
  typedef bool WireGuardTunnelService(const LPCWSTR settings);

  base::FilePath directory;
  if (!base::PathService::Get(base::DIR_EXE, &directory)) {
    return S_OK;
  }
  auto tunnel_dll_path = directory.Append(L"tunnel.dll").value();
  VLOG(1) << __func__ << ": Loading " << tunnel_dll_path;
  HMODULE tunnel_lib = LoadLibrary(tunnel_dll_path.c_str());
  if (!tunnel_lib) {
    VLOG(1) << __func__ << ": tunnel.dll not found, error: "
            << logging::SystemErrorCodeToString(
                   logging::GetLastSystemErrorCode());
    return S_OK;
  }

  WireGuardTunnelService* tunnel_proc =
      reinterpret_cast<WireGuardTunnelService*>(
          GetProcAddress(tunnel_lib, "WireGuardTunnelService"));
  if (!tunnel_proc) {
    VLOG(1) << __func__ << ": WireGuardTunnelService not found error: "
            << logging::SystemErrorCodeToString(
                   logging::GetLastSystemErrorCode());
    return S_OK;
  }

  auto config_path = directory.Append(L"brave.test.conf").value();
  VLOG(1) << __func__ << ": Brave " << config_path;
  auto result = tunnel_proc(config_path.c_str());

  if (!result) {
    VLOG(1) << __func__ << ": failed to activate tunnel service:"
            << logging::SystemErrorCodeToString(
                   logging::GetLastSystemErrorCode())
            << " -> " << std::hex << HRESULTFromLastError();
  }
  return S_OK;
}

// static
HRESULT ServiceMain::InitializeComSecurity() {
  CDacl dacl;
  constexpr auto com_rights_execute_local =
      COM_RIGHTS_EXECUTE | COM_RIGHTS_EXECUTE_LOCAL;
  if (!dacl.AddAllowedAce(Sids::System(), com_rights_execute_local) ||
      !dacl.AddAllowedAce(Sids::Admins(), com_rights_execute_local) ||
      !dacl.AddAllowedAce(Sids::Interactive(), com_rights_execute_local)) {
    return E_ACCESSDENIED;
  }

  CSecurityDesc sd;
  sd.SetDacl(dacl);
  sd.MakeAbsolute();
  sd.SetOwner(Sids::Admins());
  sd.SetGroup(Sids::Admins());

  // These are the flags being set:
  // EOAC_DYNAMIC_CLOAKING: DCOM uses the thread token (if present) when
  //   determining the client's identity. Useful when impersonating another
  //   user.
  // EOAC_SECURE_REFS: Authenticates distributed reference count calls to
  //   prevent malicious users from releasing objects that are still being used.
  // EOAC_DISABLE_AAA: Causes any activation where a server process would be
  //   launched under the caller's identity (activate-as-activator) to fail with
  //   E_ACCESSDENIED.
  // EOAC_NO_CUSTOM_MARSHAL: reduces the chances of executing arbitrary DLLs
  //   because it allows the marshaling of only CLSIDs that are implemented in
  //   Ole32.dll, ComAdmin.dll, ComSvcs.dll, or Es.dll, or that implement the
  //   CATID_MARSHALER category ID.
  // RPC_C_AUTHN_LEVEL_PKT_PRIVACY: prevents replay attacks, verifies that none
  //   of the data transferred between the client and server has been modified,
  //   ensures that the data transferred can only be seen unencrypted by the
  //   client and the server.
  return ::CoInitializeSecurity(
      const_cast<SECURITY_DESCRIPTOR*>(sd.GetPSECURITY_DESCRIPTOR()), -1,
      nullptr, nullptr, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IDENTIFY,
      nullptr,
      EOAC_DYNAMIC_CLOAKING | EOAC_DISABLE_AAA | EOAC_SECURE_REFS |
          EOAC_NO_CUSTOM_MARSHAL,
      nullptr);
}

HRESULT ServiceMain::Run() {
  VLOG(1) << __func__;
  HRESULT hr = InitializeComSecurity();
  if (FAILED(hr)) {
    return hr;
  }

  CreateWRLModule();
  hr = RegisterClassObject();
  if (SUCCEEDED(hr)) {
    CreateWGConnection();
    UnregisterClassObject();
  }

  return S_OK;
}

void ServiceMain::SignalExit() {
  VLOG(1) << __func__;
}

}  // namespace brave_vpn
