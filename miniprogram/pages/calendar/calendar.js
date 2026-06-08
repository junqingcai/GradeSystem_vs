const storage = require('../../utils/storage')
const dateUtil = require('../../utils/date')
const calculator = require('../../utils/calculator')

Page({
  data: {
    currentMonth: '',
    weekNames: ['日', '一', '二', '三', '四', '五', '六'],
    days: [],
    dailyBudgetText: '0.00'
  },

  onShow() {
    this.loadCalendar()
  },

  loadCalendar() {
    const currentMonth = dateUtil.getCurrentMonth()
    const budgets = storage.getBudgets()
    const records = storage.getRecords()
    const budget = Number((budgets[currentMonth] && budgets[currentMonth].budget) || 0)
    const daysInMonth = dateUtil.getDaysInMonth(currentMonth)
    const dailyBudget = daysInMonth > 0 ? budget / daysInMonth : 0
    const dailyExpenseMap = calculator.getDailyExpenseMap(records, currentMonth)
    const days = dateUtil.getCalendarDays(currentMonth).map(item => {
      if (item.empty) {
        return item
      }
      const expense = dailyExpenseMap[item.date] || 0
      return Object.assign({}, item, {
        expense,
        expenseText: calculator.toFixedMoney(expense),
        level: this.getLevel(expense, dailyBudget)
      })
    })

    this.setData({
      currentMonth,
      days,
      dailyBudgetText: calculator.toFixedMoney(dailyBudget)
    })
  },

  getLevel(expense, dailyBudget) {
    if (expense === 0) {
      return 'zero'
    }
    if (dailyBudget <= 0) {
      return 'red'
    }
    if (expense < dailyBudget * 0.8) {
      return 'green'
    }
    if (expense <= dailyBudget) {
      return 'yellow'
    }
    return 'red'
  },

  openDetail(e) {
    const date = e.currentTarget.dataset.date
    if (!date) {
      return
    }
    wx.navigateTo({
      url: '/pages/detail/detail?date=' + date
    })
  }
})
