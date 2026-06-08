const storage = require('../../utils/storage')
const dateUtil = require('../../utils/date')
const calculator = require('../../utils/calculator')

Page({
  data: {
    currentMonth: '',
    today: '',
    summary: {},
    recentRecords: []
  },

  onLoad() {
    this.checkBudget()
  },

  onShow() {
    this.loadData()
  },

  checkBudget() {
    const currentMonth = dateUtil.getCurrentMonth()
    const budgets = storage.getBudgets()
    if (!budgets[currentMonth] || Number(budgets[currentMonth].budget) <= 0) {
      wx.showModal({
        title: '设置本月预算',
        content: '检测到本月还没有设置生活费预算，请先设置本月预算。',
        confirmText: '去设置',
        cancelText: '稍后',
        success(res) {
          if (res.confirm) {
            wx.switchTab({ url: '/pages/settings/settings' })
          }
        }
      })
    }
  },

  loadData() {
    const currentMonth = dateUtil.getCurrentMonth()
    const today = dateUtil.getCurrentDate()
    const budgets = storage.getBudgets()
    const records = storage.getRecords()
    const summary = calculator.getMonthSummary(records, budgets, currentMonth, today)
    const recentRecords = calculator.getMonthRecords(records, currentMonth).slice(0, 5).map(item => {
      return Object.assign({}, item, {
        amountText: calculator.toFixedMoney(item.amount),
        typeText: item.type === 'expense' ? '支出' : '收入'
      })
    })

    this.setData({
      currentMonth,
      today,
      summary: this.formatSummary(summary),
      recentRecords
    })
  },

  formatSummary(summary) {
    return {
      budget: calculator.toFixedMoney(summary.budget),
      income: calculator.toFixedMoney(summary.income),
      expense: calculator.toFixedMoney(summary.expense),
      remaining: calculator.toFixedMoney(summary.remaining),
      todayExpense: calculator.toFixedMoney(summary.todayExpense),
      remainingDays: summary.remainingDays,
      averageCanSpend: calculator.toFixedMoney(summary.averageCanSpend)
    }
  },

  addExpense() {
    wx.navigateTo({
      url: '/pages/edit/edit?type=expense&date=' + this.data.today
    })
  },

  addIncome() {
    wx.navigateTo({
      url: '/pages/edit/edit?type=income&date=' + this.data.today
    })
  },

  goDetail() {
    wx.navigateTo({
      url: '/pages/detail/detail?date=' + this.data.today
    })
  }
})
