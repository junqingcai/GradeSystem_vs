function pad(num) {
  return num < 10 ? '0' + num : '' + num
}

function formatDate(date) {
  return date.getFullYear() + '-' + pad(date.getMonth() + 1) + '-' + pad(date.getDate())
}

function formatDateTime(date) {
  return formatDate(date) + ' ' + pad(date.getHours()) + ':' + pad(date.getMinutes())
}

function getCurrentDate() {
  return formatDate(new Date())
}

function getCurrentMonth() {
  const now = new Date()
  return now.getFullYear() + '-' + pad(now.getMonth() + 1)
}

function getMonthFromDate(dateText) {
  return dateText.slice(0, 7)
}

function getDaysInMonth(monthText) {
  const parts = monthText.split('-')
  const year = Number(parts[0])
  const month = Number(parts[1])
  return new Date(year, month, 0).getDate()
}

function getRemainingDays(dateText) {
  const month = getMonthFromDate(dateText)
  const todayDay = Number(dateText.slice(8, 10))
  return getDaysInMonth(month) - todayDay + 1
}

function getCalendarDays(monthText) {
  const parts = monthText.split('-')
  const year = Number(parts[0])
  const month = Number(parts[1])
  const daysInMonth = getDaysInMonth(monthText)
  const firstWeekDay = new Date(year, month - 1, 1).getDay()
  const list = []

  for (let i = 0; i < firstWeekDay; i++) {
    list.push({ empty: true })
  }

  for (let day = 1; day <= daysInMonth; day++) {
    list.push({
      empty: false,
      day,
      date: monthText + '-' + pad(day)
    })
  }

  return list
}

function toChineseDate(dateText) {
  const parts = dateText.split('-')
  return Number(parts[0]) + '年' + Number(parts[1]) + '月' + Number(parts[2]) + '日'
}

module.exports = {
  pad,
  formatDate,
  formatDateTime,
  getCurrentDate,
  getCurrentMonth,
  getMonthFromDate,
  getDaysInMonth,
  getRemainingDays,
  getCalendarDays,
  toChineseDate
}
