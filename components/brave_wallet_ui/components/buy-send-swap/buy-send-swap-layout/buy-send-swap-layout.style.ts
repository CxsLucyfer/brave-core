// Copyright (c) 2021 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at https://mozilla.org/MPL/2.0/.
import styled from 'styled-components'
import { BuySendSwapTypes } from '../../../constants/types'
import { WalletButton } from '../../shared/style'

interface StyleProps {
  isSelected: boolean
  selectedTab: BuySendSwapTypes
  tabID: BuySendSwapTypes
  isDisabled: boolean
}

export const StyledWrapper = styled.div`
  display: flex;
  height: 100%;
  width: 100%;
  flex-direction: column;
  align-items: center;
  justify-content: center;
`

export const MainContainerWrapper = styled.div<Partial<StyleProps>>`
  display: flex;
  height: 100%;
  width: 100%;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  background-color: ${(p) => p.theme.legacy.color.background01};
  border-radius: 0px 0px 8px 8px;
`

export const MainContainer = styled.div<Partial<StyleProps>>`
  position: relative;
  display: flex;
  width: 100%;
  flex-direction: column;
  align-items: center;
  justify-content: flex-start;
  padding: 18px;
  background-color: ${(p) => p.theme.legacy.color.background02};
  border: ${(p) => `2px solid ${p.theme.legacy.color.divider01}`};
  border-radius: ${(p) =>
    p.selectedTab === 'buy' ? '0px 8px 8px 8px' : p.selectedTab === 'swap' ? '8px 0px 8px 8px' : '8px'};
`

export const ButtonRow = styled.div`
  display: flex;
  height: 38px;
  width: 100%;
  flex-direction: row;
  align-items: center;
  justify-content: center;
  position: relative;
  top: 2px;
  & > * {
    min-width: 33.33%;
  }
`

export const TabButton = styled(WalletButton) <Partial<StyleProps>>`
  flex: 1;
  display: flex;
  width: 100%;
  height: 100%;
  min-height: 40px;
  align-items: center;
  justify-content: center;
  cursor: ${(p) => p.isDisabled ? 'default' : 'pointer'};
  outline: none;
  background: ${(p) =>
    p.isSelected ? p.theme.legacy.color.background02 : p.theme.legacy.color.background01};
  border-radius: 12px 12px 0px 0px;
  border: ${(p) =>
    p.isSelected ? `2px solid ${p.theme.legacy.color.divider01}` : `2px solid ${p.theme.legacy.color.background01}`};
  border-bottom-width: 0px;
  z-index: ${(p) =>
    p.isSelected ? '2' : '0'};
  position: relative;
  pointer-events: ${(p) => p.disabled ? 'none' : 'auto'};
`

export const TabButtonText = styled.span<Partial<StyleProps>>`
  font-family: Poppins;
  font-size: 14px;
  line-height: 20px;
  font-weight: 600;
  letter-spacing: 0.01em;
  background: ${(p) =>
    p.isDisabled ? p.theme.legacy.color.interactive08 : p.isSelected ? p.theme.legacy.color.text01 : p.theme.legacy.color.text02};
  background-clip: text;
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
`

export const RightDivider = styled.div<Partial<StyleProps>>`
  height: 20px;
  width: 1px;
  background-color: ${(p) => p.selectedTab === 'buy' && p.tabID === 'send' ? p.theme.legacy.color.divider01 : p.selectedTab === 'swap' && p.tabID === 'buy' ? p.theme.legacy.color.divider01 : 'none'};
  position: absolute;
  right: -2px;
  bottom: 4px;
`

export const LeftDivider = styled.div<Partial<StyleProps>>`
  height: 20px;
  width: 1px;
  background-color: ${(p) => p.selectedTab === 'buy' && p.tabID === 'swap' ? p.theme.legacy.color.divider01 : p.selectedTab === 'swap' && p.tabID === 'send' ? p.theme.legacy.color.divider01 : 'none'};
  position: absolute;
  left: -2px;
  bottom: 4px;
`

export const ButtonWrapper = styled.div`
  display: flex;
  position: relative;
  width: 100%;
  height: 100%;
`

export const HelpCenterText = styled.div`
  font-family: Poppins;
  font-size: 13px;
  line-height: 12px;
  font-weight: 400;
  letter-spacing: 0.01em;
  color: ${p => p.theme.legacy.color.text01};
  margin-top: 20px;
`

export const HelpCenterLink = styled.a`
  color: ${p => p.theme.legacy.palette.blurple500};
  text-decoration: none;
`
