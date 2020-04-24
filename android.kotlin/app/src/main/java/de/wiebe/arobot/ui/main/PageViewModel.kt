package de.wiebe.arobot.ui.main

import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import de.wiebe.arobot.App
import kotlinx.coroutines.launch
import org.json.JSONArray


class PageViewModel : ViewModel() {
    var status = MutableLiveData<Any>()
    var connected = MutableLiveData<Boolean>()
    var sent = MutableLiveData<String>()

    var windows = MutableLiveData<JSONArray>()

    fun connect() {
        viewModelScope.launch {
            var err: String? = null
            val json: String = try {
                App.requestShutters("/status")
                // FIXME: parse status
            } catch (e: Throwable) {
                err = e.message
                ""
            }

            connected.value = json.isNotEmpty()
            status.value = json
//            connected_err.value = err
        }
    }

    fun getWindows() = viewModelScope.launch {
        try {
            windows.value = JSONArray(App.requestShutters("/window?action=list"))
        } catch (e: Throwable) {}
    }
}
