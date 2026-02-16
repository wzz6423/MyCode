<template>
  <NotifyContainer />
  <router-view />
</template>

<script setup lang="ts">
import NotifyContainer from './components/NotifyContainer.vue'
</script>

<style>
:root {
  --color-primary: #356bfb;
  --color-primary-light: #5a8afc;
  --color-primary-bg: rgba(53, 107, 251, 0.06);
  --color-text-1: #303040;
  --color-text-2: #5c5c72;
  --color-text-3: #9898aa;
  --color-border: #e8e8ef;
  --color-bg-card: rgba(255, 255, 255, 0.55);
  --color-bg-hover: rgba(255, 255, 255, 0.7);
  --shadow-sm: 0 1px 3px rgba(0, 0, 0, 0.03), 0 1px 2px rgba(0, 0, 0, 0.015);
  --shadow-md: 0 4px 16px rgba(0, 0, 0, 0.04), 0 1px 4px rgba(0, 0, 0, 0.02);
  --shadow-lg: 0 8px 30px rgba(0, 0, 0, 0.06), 0 2px 8px rgba(0, 0, 0, 0.02);
  --radius-sm: 8px;
  --radius-md: 12px;
  --radius-lg: 16px;
}

html, body, #app {
  margin: 0;
  padding: 0;
  height: 100%;
  font-family: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', Arial, sans-serif;
  color: var(--color-text-1);
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
}

/* ===== Selection ===== */
::selection {
  background: rgba(53, 107, 251, 0.12);
  color: var(--color-text-1);
}

/* ===== Scrollbar ===== */
::-webkit-scrollbar {
  width: 6px;
  height: 6px;
}
::-webkit-scrollbar-track {
  background: transparent;
}
::-webkit-scrollbar-thumb {
  background: rgba(0, 0, 0, 0.12);
  border-radius: 3px;
}
::-webkit-scrollbar-thumb:hover {
  background: rgba(0, 0, 0, 0.2);
}

/* ===== Dialog ===== */
.el-dialog {
  border-radius: var(--radius-lg) !important;
  overflow: hidden;
  box-shadow: var(--shadow-lg), 0 0 0 1px rgba(0, 0, 0, 0.03) !important;
}
.el-dialog__header {
  padding: 20px 24px 16px !important;
}
.el-dialog__title {
  font-weight: 600 !important;
  font-size: 16px !important;
  color: var(--color-text-1) !important;
}
.el-dialog__body {
  padding: 0 24px 20px !important;
}
.el-dialog__footer {
  padding: 12px 24px 20px !important;
}

/* MessageBox */
.el-message-box {
  border-radius: var(--radius-lg) !important;
  box-shadow: var(--shadow-lg) !important;
}

/* ===== Buttons ===== */
.el-button {
  border-radius: 20px !important;
  font-weight: 500 !important;
  transition: all 0.2s ease !important;
}
.el-button--primary:not(.is-link):not(.is-text) {
  background: var(--color-primary) !important;
  border-color: var(--color-primary) !important;
}
.el-button--primary:not(.is-link):not(.is-text):hover,
.el-button--primary:not(.is-link):not(.is-text):focus {
  background: var(--color-primary-light) !important;
  border-color: var(--color-primary-light) !important;
}
.el-button.is-link {
  background: transparent !important;
  border: none !important;
  box-shadow: none !important;
  border-radius: 0 !important;
}
.el-button--small {
  border-radius: 16px !important;
}
.el-button.is-round {
  border-radius: 20px !important;
}

/* ===== Inputs ===== */
.el-input__wrapper {
  border-radius: 10px !important;
  background: #fff !important;
  box-shadow: 0 0 0 1px var(--color-border) inset !important;
  transition: all 0.2s ease !important;
}
.el-input__wrapper:hover {
  box-shadow: 0 0 0 1px #c8c8d4 inset !important;
}
.el-input__wrapper.is-focus {
  box-shadow: 0 0 0 1px var(--color-primary) inset, 0 0 0 3px rgba(53, 107, 251, 0.08) !important;
}
.el-textarea__inner {
  border-radius: 10px !important;
  background: #fff !important;
  box-shadow: 0 0 0 1px var(--color-border) inset !important;
  transition: all 0.2s ease !important;
}
.el-textarea__inner:hover {
  box-shadow: 0 0 0 1px #c8c8d4 inset !important;
}
.el-textarea__inner:focus {
  box-shadow: 0 0 0 1px var(--color-primary) inset, 0 0 0 3px rgba(53, 107, 251, 0.08) !important;
}

/* ===== Select ===== */
.el-select__wrapper {
  border-radius: 10px !important;
}
.el-select-dropdown {
  border-radius: 10px !important;
  box-shadow: var(--shadow-lg) !important;
  border: 1px solid var(--color-border) !important;
}

/* ===== Dropdown ===== */
.el-dropdown-menu {
  border-radius: 10px !important;
  box-shadow: var(--shadow-lg) !important;
  border: 1px solid var(--color-border) !important;
  padding: 4px !important;
}
.el-dropdown-menu__item {
  border-radius: 6px !important;
  margin: 1px 0 !important;
  transition: background 0.15s ease !important;
}

/* ===== Tags ===== */
.el-tag {
  border-radius: 12px !important;
}

/* ===== Table ===== */
.el-table {
  border-radius: var(--radius-md) !important;
  overflow: hidden;
  color: var(--color-text-1) !important;
  font-size: 13px !important;
}
.el-table__inner-wrapper::before {
  display: none;
}
.el-table .el-table__cell {
  border-bottom: none !important;
  padding: 10px 0 !important;
}
.el-table--border::after,
.el-table--group::after,
.el-table::before {
  display: none !important;
}
.el-table th.el-table__cell {
  color: var(--color-text-3) !important;
  font-weight: 500 !important;
  font-size: 12px !important;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

/* ===== Checkbox ===== */
.el-checkbox__inner {
  border-radius: 4px !important;
  background: #fff !important;
  border-color: var(--color-border) !important;
  transition: all 0.15s ease !important;
}
.el-checkbox__input.is-checked .el-checkbox__inner,
.el-checkbox__input.is-indeterminate .el-checkbox__inner {
  background: var(--color-primary) !important;
  border-color: var(--color-primary) !important;
}

/* ===== Pagination ===== */
.el-pagination .el-pager li {
  border-radius: 8px !important;
}
.el-pagination .btn-prev,
.el-pagination .btn-next {
  border-radius: 8px !important;
}

/* ===== Upload ===== */
.el-upload-dragger {
  border-radius: var(--radius-md) !important;
}
.el-upload--text {
  border-radius: var(--radius-md) !important;
}

/* ===== Popover / Tooltip ===== */
.el-popover.el-popper {
  border-radius: 10px !important;
  box-shadow: var(--shadow-lg) !important;
}
.el-tooltip__popper {
  border-radius: var(--radius-sm) !important;
}

/* ===== Menu ===== */
.el-menu-item {
  border-radius: var(--radius-sm) !important;
  margin: 2px 8px !important;
}

/* ===== Loading ===== */
.el-loading-mask {
  border-radius: inherit;
}

/* ===== Notification / Message ===== */
.el-notification {
  border-radius: var(--radius-md) !important;
}
.el-message {
  border-radius: 10px !important;
}

/* ===== Tabs ===== */
.el-tabs__item {
  font-weight: 500 !important;
  transition: color 0.2s ease !important;
}
.el-tabs__item.is-active {
  color: var(--color-primary) !important;
}
.el-tabs__active-bar {
  background-color: var(--color-primary) !important;
}
</style>
