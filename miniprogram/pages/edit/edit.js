const storage = require('../../utils/storage')
const dateUtil = require('../../utils/date')

Page({
  data: {
    isEdit: false,
    id: 0,
    type: 'expense',
    typeIndex: 0,
    typeOptions: [
      { label: '支出', value: 'expense' },
      { label: '收入', value: 'income' }
    ],
    date: '',
    amount: '',
    category: '',
    categoryIndex: 0,
    categories: [],
    remark: ''
  },

  onLoad(options) {
    const categoriesData = storage.getCategories()
    const id = Number(options.id || 0)

    if (id) {
      const record = storage.getRecordById(id)
      if (!record) {
        wx.showToast({ title: '记录不存在', icon: 'none' })
        wx.navigateBack()
        return
      }
      const categories = categoriesData[record.type]
      this.setData({
        isEdit: true,
        id: record.id,
        type: record.type,
        typeIndex: record.type === 'expense' ? 0 : 1,
        date: record.date,
        amount: String(record.amount),
        category: record.category,
        categoryIndex: Math.max(categories.indexOf(record.category), 0),
        categories,
        remark: record.remark
      })
    } else {
      const type = options.type || 'expense'
      const categories = categoriesData[type]
      this.setData({
        type,
        typeIndex: type === 'expense' ? 0 : 1,
        date: options.date || dateUtil.getCurrentDate(),
        categories,
        category: categories[0],
        categoryIndex: 0
      })
    }
  },

  changeType(e) {
    const typeIndex = Number(e.detail.value)
    const type = this.data.typeOptions[typeIndex].value
    const categories = storage.getCategories()[type]
    this.setData({
      typeIndex,
      type,
      categories,
      categoryIndex: 0,
      category: categories[0]
    })
  },

  changeDate(e) {
    this.setData({ date: e.detail.value })
  },

  changeCategory(e) {
    const categoryIndex = Number(e.detail.value)
    this.setData({
      categoryIndex,
      category: this.data.categories[categoryIndex]
    })
  },

  inputAmount(e) {
    this.setData({ amount: e.detail.value })
  },

  inputRemark(e) {
    this.setData({ remark: e.detail.value })
  },

  saveRecord() {
    const amount = Number(this.data.amount)
    if (!amount || amount <= 0) {
      wx.showToast({ title: '请输入正确金额', icon: 'none' })
      return
    }
    if (!this.data.category) {
      wx.showToast({ title: '请选择分类', icon: 'none' })
      return
    }

    const record = {
      id: this.data.isEdit ? this.data.id : Date.now(),
      type: this.data.type,
      date: this.data.date,
      amount,
      category: this.data.category,
      remark: this.data.remark,
      createTime: this.data.isEdit ? storage.getRecordById(this.data.id).createTime : dateUtil.formatDateTime(new Date())
    }

    if (this.data.isEdit) {
      storage.updateRecord(record)
    } else {
      storage.addRecord(record)
    }

    wx.showToast({ title: '已保存', icon: 'success' })
    setTimeout(() => {
      wx.navigateBack()
    }, 500)
  }
})
