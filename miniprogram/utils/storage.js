const BUDGETS_KEY = 'budgets'
const RECORDS_KEY = 'records'
const CATEGORIES_KEY = 'categories'

const defaultCategories = {
  expense: ['餐饮', '交通', '学习', '购物', '娱乐', '日用品', '医疗', '通讯', '其他'],
  income: ['生活费', '兼职', '退款', '红包', '其他收入']
}

function initStorage() {
  if (!wx.getStorageSync(BUDGETS_KEY)) {
    wx.setStorageSync(BUDGETS_KEY, {})
  }
  if (!wx.getStorageSync(RECORDS_KEY)) {
    wx.setStorageSync(RECORDS_KEY, [])
  }
  if (!wx.getStorageSync(CATEGORIES_KEY)) {
    wx.setStorageSync(CATEGORIES_KEY, defaultCategories)
  }
}

function getBudgets() {
  return wx.getStorageSync(BUDGETS_KEY) || {}
}

function saveBudgets(budgets) {
  wx.setStorageSync(BUDGETS_KEY, budgets)
}

function getRecords() {
  return wx.getStorageSync(RECORDS_KEY) || []
}

function saveRecords(records) {
  wx.setStorageSync(RECORDS_KEY, records)
}

function getCategories() {
  return wx.getStorageSync(CATEGORIES_KEY) || defaultCategories
}

function saveCategories(categories) {
  wx.setStorageSync(CATEGORIES_KEY, categories)
}

function addRecord(record) {
  const records = getRecords()
  records.unshift(record)
  saveRecords(records)
}

function updateRecord(record) {
  const records = getRecords().map(item => {
    return item.id === record.id ? record : item
  })
  saveRecords(records)
}

function deleteRecord(id) {
  const records = getRecords().filter(item => item.id !== id)
  saveRecords(records)
}

function getRecordById(id) {
  const records = getRecords()
  return records.find(item => item.id === id)
}

module.exports = {
  defaultCategories,
  initStorage,
  getBudgets,
  saveBudgets,
  getRecords,
  saveRecords,
  getCategories,
  saveCategories,
  addRecord,
  updateRecord,
  deleteRecord,
  getRecordById
}
