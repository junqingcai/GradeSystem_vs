const storage = require('../../utils/storage')
const dateUtil = require('../../utils/date')
const calculator = require('../../utils/calculator')

Page({
  data: {
    currentMonth: '',
    statistics: {}
  },

  onShow() {
    this.loadStatistics()
  },

  loadStatistics() {
    const currentMonth = dateUtil.getCurrentMonth()
    const statistics = calculator.getStatistics(storage.getRecords(), storage.getBudgets(), currentMonth)
    const formatted = {
      expense: calculator.toFixedMoney(statistics.expense),
      income: calculator.toFixedMoney(statistics.income),
      remaining: calculator.toFixedMoney(statistics.remaining),
      averageDailyExpense: calculator.toFixedMoney(statistics.averageDailyExpense),
      overBudgetDays: statistics.overBudgetDays,
      maxDayDate: statistics.maxDay.date || '暂无',
      maxDayAmount: calculator.toFixedMoney(statistics.maxDay.amount),
      categoryRank: statistics.categoryRank.map(item => {
        return {
          category: item.category,
          amountText: calculator.toFixedMoney(item.amount),
          percentText: item.percent.toFixed(0)
        }
      })
    }

    this.setData({
      currentMonth,
      statistics: formatted
    })
  }
})
