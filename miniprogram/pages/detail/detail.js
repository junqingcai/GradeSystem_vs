const storage = require('../../utils/storage')
const dateUtil = require('../../utils/date')
const calculator = require('../../utils/calculator')

Page({
  data: {
    date: '',
    dateTitle: '',
    expenseText: '0.00',
    incomeText: '0.00',
    groups: []
  },

  onLoad(options) {
    const date = options.date || dateUtil.getCurrentDate()
    this.setData({
      date,
      dateTitle: dateUtil.toChineseDate(date)
    })
  },

  onShow() {
    this.loadDetail()
  },

  loadDetail() {
    const records = calculator.getDateRecords(storage.getRecords(), this.data.date)
    const expense = calculator.sumByType(records, 'expense')
    const income = calculator.sumByType(records, 'income')
    const groups = calculator.groupByCategory(records).map(group => {
      return {
        category: group.category,
        records: group.records.map(item => {
          return Object.assign({}, item, {
            amountText: calculator.toFixedMoney(item.amount),
            typeText: item.type === 'expense' ? '支出' : '收入'
          })
        })
      }
    })

    this.setData({
      expenseText: calculator.toFixedMoney(expense),
      incomeText: calculator.toFixedMoney(income),
      groups
    })
  },

  addExpense() {
    wx.navigateTo({
      url: '/pages/edit/edit?type=expense&date=' + this.data.date
    })
  },

  addIncome() {
    wx.navigateTo({
      url: '/pages/edit/edit?type=income&date=' + this.data.date
    })
  },

  editRecord(e) {
    const id = e.currentTarget.dataset.id
    wx.navigateTo({
      url: '/pages/edit/edit?id=' + id
    })
  },

  deleteRecord(e) {
    const id = Number(e.currentTarget.dataset.id)
    wx.showModal({
      title: '确认删除',
      content: '删除后无法恢复，确定删除这条记录吗？',
      confirmText: '删除',
      confirmColor: '#c62828',
      success: res => {
        if (res.confirm) {
          storage.deleteRecord(id)
          wx.showToast({ title: '已删除', icon: 'success' })
          this.loadDetail()
        }
      }
    })
  }
})
