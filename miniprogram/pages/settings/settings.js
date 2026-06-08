const storage = require('../../utils/storage')
const dateUtil = require('../../utils/date')
const calculator = require('../../utils/calculator')

Page({
  data: {
    currentMonth: '',
    budgetInput: '',
    categories: { expense: [], income: [] },
    newExpenseCategory: '',
    newIncomeCategory: ''
  },

  onShow() {
    this.loadSettings()
  },

  loadSettings() {
    const currentMonth = dateUtil.getCurrentMonth()
    const budgets = storage.getBudgets()
    const budget = (budgets[currentMonth] && budgets[currentMonth].budget) || ''

    this.setData({
      currentMonth,
      budgetInput: budget === '' ? '' : calculator.toFixedMoney(budget),
      categories: storage.getCategories()
    })
  },

  inputBudget(e) {
    this.setData({ budgetInput: e.detail.value })
  },

  saveBudget() {
    const budget = Number(this.data.budgetInput)
    if (budget < 0 || this.data.budgetInput === '') {
      wx.showToast({ title: '请输入正确预算', icon: 'none' })
      return
    }
    const budgets = storage.getBudgets()
    budgets[this.data.currentMonth] = { budget }
    storage.saveBudgets(budgets)
    wx.showToast({ title: '预算已保存', icon: 'success' })
    this.loadSettings()
  },

  inputExpenseCategory(e) {
    this.setData({ newExpenseCategory: e.detail.value })
  },

  inputIncomeCategory(e) {
    this.setData({ newIncomeCategory: e.detail.value })
  },

  addExpenseCategory() {
    this.addCategory('expense', this.data.newExpenseCategory)
  },

  addIncomeCategory() {
    this.addCategory('income', this.data.newIncomeCategory)
  },

  addCategory(type, name) {
    const categoryName = name.trim()
    if (!categoryName) {
      wx.showToast({ title: '请输入分类名称', icon: 'none' })
      return
    }

    const categories = storage.getCategories()
    if (categories[type].indexOf(categoryName) !== -1) {
      wx.showToast({ title: '分类已存在', icon: 'none' })
      return
    }

    categories[type].push(categoryName)
    storage.saveCategories(categories)
    wx.showToast({ title: '分类已添加', icon: 'success' })
    this.setData({
      newExpenseCategory: type === 'expense' ? '' : this.data.newExpenseCategory,
      newIncomeCategory: type === 'income' ? '' : this.data.newIncomeCategory
    })
    this.loadSettings()
  }
})
