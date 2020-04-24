package de.wiebe.arobot

import android.content.Context
import android.content.SharedPreferences
import androidx.preference.PreferenceManager


const val SHUTTERS_ADDRESS = "shutters_address"
const val XMAS_ADDRESS = "xmas_address"

class Prefs (context: Context) {
  private val prefs: SharedPreferences = PreferenceManager.getDefaultSharedPreferences(context)

  var shuttersAddress
    get() = prefs.getString(SHUTTERS_ADDRESS, "http://192.168.1.51:8080")
    set(value) = prefs.edit().putString(SHUTTERS_ADDRESS, value).apply()

  var xmasAddress
    get() = prefs.getString(XMAS_ADDRESS, "http://192.168.1.11:80")
    set(value) = prefs.edit().putString(XMAS_ADDRESS, value).apply()
}