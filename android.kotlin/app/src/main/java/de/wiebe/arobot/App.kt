package de.wiebe.arobot

import android.app.Application
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.net.URL

class App : Application() {
    companion object {
        private var prefs: Prefs? = null

        suspend fun requestShutters(url: String): String = withContext(Dispatchers.IO) {
            URL("${prefs!!.shuttersAddress}$url").readText()
        }

        suspend fun requestXmas(url: String): String = withContext(Dispatchers.IO) {
            URL("${prefs!!.xmasAddress}$url").readText()
        }
    }

    override fun onCreate() {
        prefs = Prefs(applicationContext)
        super.onCreate()
    }
}