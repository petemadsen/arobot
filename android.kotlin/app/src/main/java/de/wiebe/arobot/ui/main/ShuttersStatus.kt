package de.wiebe.arobot.ui.main

import org.json.JSONObject

class ShuttersStatus(json: String) : JSONObject(json) {
    val app = App(this.optJSONObject("app"))
//    val tempOut: String = this.optString("temp_out")
    val local = Local(this.optJSONObject("local"))
}


class App(json: JSONObject) {
    val uptime: String = json.optString("uptime")
}


class Local(json: JSONObject?) {
    val tempOut: Double? = json?.optDouble("temp_out")
//    val tempIn: Double? = json?.optDouble("temp_in")
}