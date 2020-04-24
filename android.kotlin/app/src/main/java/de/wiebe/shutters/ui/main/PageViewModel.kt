package de.wiebe.shutters.ui.main

import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import de.wiebe.shutters.App
import kotlinx.coroutines.launch
import org.json.JSONArray

class Xmas(val numModes: Int, val mode: Int, val desc: List<String>)


class PageViewModel : ViewModel() {
    var status = MutableLiveData<Any>()
    var connected = MutableLiveData<Boolean>()
    var sent = MutableLiveData<String>()
    var editContact = MutableLiveData<Contact>()
    var delContact = MutableLiveData<Contact>()

    var contacts = MutableLiveData<List<Contact>>()

    var windows = MutableLiveData<JSONArray>()

    var phoneContacts = MutableLiveData<Map<String,String>>()

    var xmas = MutableLiveData<Xmas>()

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

        viewModelScope.launch {
            val status: String = try {
                App.requestXmas("/status").trim()
            } catch (e: Throwable) {
                ""
            }
            println("--xmas_status: $status #${status.length}")
            val splitSep = "\\s+".toRegex()
            val fields = status.split(splitSep)
            println("--field-size: ${fields.size}")

            var mode = -1
            var numModes = -1

            if ((fields.size % 2) == 0) {
                for (x in fields.indices step 2) {
                    println("--status/${fields[x]}/${fields[x+1]}/")
                    try {
                        when (fields[x]) {
                            "mode" -> mode = fields[x + 1].toInt()
                            "modes" -> numModes = fields[x + 1].toInt()
                        }
                    } catch (e: Throwable) {}
                }
            }

            val modeDesc = mutableListOf<String>()
            for (x in 0 until numModes) {
                val desc: String = try {
                    App.requestXmas("/modedesc?$x")
                } catch (e: Throwable) {
                    x.toString()
                }
                modeDesc.add(desc)
            }

            xmas.value = Xmas(numModes, mode, modeDesc)
        }
    }

    fun getContacts() {
        viewModelScope.launch {
            val entries = mutableListOf<Contact>()

            try {
                println("--1")
                val jsonNumbers = App.requestShutters("/phone/unknown")
                val newnumbers = JSONArray(jsonNumbers)
                for (i in 0 until newnumbers.length()) {
                    val number = newnumbers.getString(i)
                    entries.add(Contact(number, "", getPhoneName(number)))
                }

                println("--2")
                val jsonPhonebook = App.requestShutters("/phone/book")
                val oldNumbers = PhoneBook(jsonPhonebook)
                oldNumbers.keys().forEach { number ->
                    entries.add(Contact(number, oldNumbers.getString(number), getPhoneName(number)))
                }

                println("--3")

            } catch (e: Throwable) {
                println("--key-bad: ${e.message}")
            }

            contacts.value = entries
        }
    }

    fun getWindows() = viewModelScope.launch {
        try {
            windows.value = JSONArray(App.requestShutters("/window?action=list"))
        } catch (e: Throwable) {}
    }

    fun sendContact(contact: Contact)
    {
        viewModelScope.launch {
            val msg = try {
                val reply = App.requestShutters("/phone/add?number=${contact.number}&name=${contact.name}")
                reply //if (reply == "OK") { getString(R.string.saved) } else { getString(R.string.err_saved) }
            } catch (e: Exception) {
                "FIXME: failed"//getString(R.string.err_saved)
            }

            sent.value = msg
        }
    }

    fun deleteContact(contact: Contact)
    {
        viewModelScope.launch {
            val msg = try {
                val reply = App.requestShutters("/phone/del?number=${contact.number}")
                reply //getString(R.string.deleted)
            } catch (e: Exception) {
                "--bad-delete"
            }

            sent.value = msg
        }
    }

    private fun getPhoneName(number: String): String {
        if (phoneContacts != null && phoneContacts.value != null)
            return phoneContacts.value!!.getOrDefault(number, "")
        return ""
    }

    fun setXmasMode(mode: String) {
        viewModelScope.launch {
            println("--set-mode: $mode")

            try {
                App.requestXmas("/mode?$mode")
            } catch (e: Throwable) {}

//            xmasMode.value = try {
//                App.requestXmas("/mode").toInt()
//            } catch(e: Throwable) {
//                -1
//            }
        }
    }
}
