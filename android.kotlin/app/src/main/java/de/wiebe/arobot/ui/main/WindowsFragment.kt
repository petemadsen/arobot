package de.wiebe.arobot.ui.main

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.appcompat.app.AlertDialog
import androidx.fragment.app.Fragment
import androidx.lifecycle.Observer
import androidx.lifecycle.ViewModelProviders
import androidx.lifecycle.lifecycleScope
import de.wiebe.arobot.App
import de.wiebe.arobot.R
import kotlinx.android.synthetic.main.fragment_windows.*
import kotlinx.android.synthetic.main.fragment_windows.view.*
import kotlinx.coroutines.launch
import org.json.JSONArray


class WindowsFragment : Fragment() {
    private lateinit var pageViewModel: PageViewModel
    private var windowsList = mutableMapOf<Int, String>()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        pageViewModel = ViewModelProviders.of(requireActivity()).get(PageViewModel::class.java)
    }

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        println("--windows-fragment-create")
        val root = inflater.inflate(R.layout.fragment_windows, container, false)

        root.btn_open.setOnClickListener { sendWindow("open") }
        root.btn_stop.setOnClickListener { sendWindow("stop") }
        root.btn_close.setOnClickListener{ sendWindow("close") }

        root.btnWindow.setOnClickListener {
            val items = windowsList.values.toTypedArray()

            val builder = AlertDialog.Builder(container!!.context)
            builder.setTitle(R.string.tab_windows)
                .setItems(items) { _, which ->
                    btnWindow.text = items[which]
                    selectWindow()
                }
            builder.create()
            builder.show()
        }

        root.btnAll.setOnCheckedChangeListener { _, _ ->
            selectWindow()
        }

        return root
    }

    private fun getSelectedWindow(): String {
        if (btnAll.isChecked)
            return "all"
        windowsList.forEach {(id, text) ->
            if (btnWindow.text == text)
                return id.toString()
        }
        return ""
    }

    private fun selectWindow() {
        val wnd = getSelectedWindow()

        btn_open.isEnabled = wnd.isNotEmpty()
        btn_close.isEnabled = wnd.isNotEmpty()
        btn_stop.isEnabled = wnd.isNotEmpty()

        btnAll.isChecked = wnd == "all"
        btnWindow.isEnabled = wnd != "all"

        lblSelection.text = when(wnd) {
            "" -> getString(R.string.select_window)
            "all" -> getString(R.string.all_windows)
            else -> btnWindow.text
        }
    }

    override fun onActivityCreated(savedInstanceState: Bundle?) {
        super.onActivityCreated(savedInstanceState)

        pageViewModel.connected.observe(viewLifecycleOwner, Observer<Boolean> {connected ->
            btnWindow.isEnabled = connected
            btnAll.isEnabled = connected
            lblConnected.text = if(connected) getString(R.string.connected) else getString(R.string.no_connection)
            pageViewModel.getWindows()
        })

        pageViewModel.status.observe(viewLifecycleOwner, Observer<Any> {
            lblTemperature.text = try { "${ShuttersStatus(it.toString()).local.tempOut} °C" } catch (e: Exception) { "-" }
        })

        pageViewModel.windows.observe(viewLifecycleOwner, Observer<JSONArray> {array ->
            windowsList.clear()
            for (i in 0 until array.length()) {
                val item = array.getJSONArray(i)
                windowsList[item.getInt(0)] = item.getString(2)
            }
        })
    }

    private fun sendWindow(action: String) {
        val wnd = getSelectedWindow()
        if (wnd.isEmpty())
            return

        val cmd = "window?window=${wnd}&action=${action}"

        lifecycleScope.launch {
            val results = try {
                App.requestShutters("/$cmd")
            } catch (e: Throwable) {
                "[${e.message}]"
            }

            pageViewModel.sent.value = getString(R.string.sent)

            lbl_status.text = "$cmd | $results"
            btnAll.isChecked = false
            btnWindow.text = getString(R.string.pls_select)

            selectWindow()
        }
    }

    companion object {
        /**
         * The fragment argument representing the section number for this
         * fragment.
         */
        private const val ARG_SECTION_NUMBER = "section_number"

        /**
         * Returns a new instance of this fragment for the given section
         * number.
         */
        @JvmStatic
        fun newInstance(sectionNumber: Int): WindowsFragment {
            return WindowsFragment().apply {
                arguments = Bundle().apply {
                    putInt(ARG_SECTION_NUMBER, sectionNumber)
                }
            }
        }
    }
}