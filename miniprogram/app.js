const storage = require('./utils/storage')
const dateUtil = require('./utils/date')

App({
  onLaunch() {
    storage.initStorage()

    const currentMonth = dateUtil.getCurrentMonth()
    const budgets = storage.getBudgets()
    if (!budgets[currentMonth]) {
      budgets[currentMonth] = { budget: 0 }
      storage.saveBudgets(budgets)
    }
  }
})
