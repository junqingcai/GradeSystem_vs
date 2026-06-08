const dateUtil = require('./date')

function toFixedMoney(value) {
  const num = Number(value) || 0
  return num.toFixed(2)
}

function getMonthRecords(records, monthText) {
  return records.filter(item => item.date.slice(0, 7) === monthText)
}

function getDateRecords(records, dateText) {
  return records.filter(item => item.date === dateText)
}

function sumByType(records, type) {
  return records.reduce((total, item) => {
    if (item.type === type) {
      return total + Number(item.amount)
    }
    return total
  }, 0)
}

function getMonthSummary(records, budgets, monthText, todayText) {
  const monthRecords = getMonthRecords(records, monthText)
  const todayRecords = getDateRecords(records, todayText)
  const budget = Number((budgets[monthText] && budgets[monthText].budget) || 0)
  const income = sumByType(monthRecords, 'income')
  const expense = sumByType(monthRecords, 'expense')
  const remaining = budget + income - expense
  const todayExpense = sumByType(todayRecords, 'expense')
  const remainingDays = dateUtil.getRemainingDays(todayText)
  const averageCanSpend = remainingDays > 0 ? remaining / remainingDays : 0

  return {
    budget,
    income,
    expense,
    remaining,
    todayExpense,
    remainingDays,
    averageCanSpend
  }
}

function getDailyExpenseMap(records, monthText) {
  const monthRecords = getMonthRecords(records, monthText)
  const map = {}

  monthRecords.forEach(item => {
    if (item.type === 'expense') {
      map[item.date] = (map[item.date] || 0) + Number(item.amount)
    }
  })

  return map
}

function groupByCategory(records) {
  const groups = []
  const map = {}

  records.forEach(item => {
    if (!map[item.category]) {
      map[item.category] = {
        category: item.category,
        records: []
      }
      groups.push(map[item.category])
    }
    map[item.category].records.push(item)
  })

  return groups
}

function getCategoryRank(records) {
  const expenseRecords = records.filter(item => item.type === 'expense')
  const totalExpense = sumByType(expenseRecords, 'expense')
  const map = {}

  expenseRecords.forEach(item => {
    map[item.category] = (map[item.category] || 0) + Number(item.amount)
  })

  return Object.keys(map).map(category => {
    const amount = map[category]
    const percent = totalExpense > 0 ? amount / totalExpense * 100 : 0
    return {
      category,
      amount,
      percent
    }
  }).sort((a, b) => b.amount - a.amount)
}

function getStatistics(records, budgets, monthText) {
  const monthRecords = getMonthRecords(records, monthText)
  const budget = Number((budgets[monthText] && budgets[monthText].budget) || 0)
  const expense = sumByType(monthRecords, 'expense')
  const income = sumByType(monthRecords, 'income')
  const remaining = budget + income - expense
  const daysInMonth = dateUtil.getDaysInMonth(monthText)
  const averageDailyExpense = daysInMonth > 0 ? expense / daysInMonth : 0
  const dailyBudget = daysInMonth > 0 ? budget / daysInMonth : 0
  const dailyExpenseMap = getDailyExpenseMap(records, monthText)
  let overBudgetDays = 0
  let maxDay = { date: '', amount: 0 }

  Object.keys(dailyExpenseMap).forEach(date => {
    const amount = dailyExpenseMap[date]
    if (dailyBudget > 0 && amount > dailyBudget) {
      overBudgetDays++
    }
    if (amount > maxDay.amount) {
      maxDay = { date, amount }
    }
  })

  return {
    budget,
    expense,
    income,
    remaining,
    averageDailyExpense,
    overBudgetDays,
    maxDay,
    categoryRank: getCategoryRank(monthRecords)
  }
}

module.exports = {
  toFixedMoney,
  getMonthRecords,
  getDateRecords,
  sumByType,
  getMonthSummary,
  getDailyExpenseMap,
  groupByCategory,
  getCategoryRank,
  getStatistics
}
