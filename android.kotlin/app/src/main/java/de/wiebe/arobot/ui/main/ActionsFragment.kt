package de.wiebe.arobot.ui.main

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import androidx.lifecycle.Observer
import androidx.lifecycle.ViewModelProviders
import androidx.lifecycle.lifecycleScope
import de.wiebe.arobot.App
import de.wiebe.arobot.R
import kotlinx.android.synthetic.main.fragment_windows.*
import kotlinx.android.synthetic.main.fragment_windows.view.*
import kotlinx.coroutines.launch


class ActionsFragment : Fragment() {
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
        val root = inflater.inflate(R.layout.fragment_windows, container, false)

        root.btn_left.setOnClickListener { sendAction("qqq") }
        root.btn_right.setOnClickListener { sendAction("aaa") }
        root.btn_up.setOnClickListener { sendAction("www") }
        root.btn_down.setOnClickListener { sendAction("sss") }
        root.btn_on.setOnClickListener { sendAction("zzz") }
        root.btn_off.setOnClickListener { sendAction("hhh") }

        return root
    }

    override fun onActivityCreated(savedInstanceState: Bundle?) {
        super.onActivityCreated(savedInstanceState)

        pageViewModel.connected.observe(viewLifecycleOwner, Observer<Boolean> {connected ->
            lblConnected.text = if(connected) getString(R.string.connected) else getString(R.string.no_connection)
            btn_down.isEnabled = connected;
            btn_up.isEnabled = connected;
            btn_left.isEnabled = connected;
            btn_right.isEnabled = connected;
            btn_on.isEnabled = connected;
            btn_off.isEnabled = connected;
        })
    }

    private fun sendAction(action: String) {
        val cmd = "action?${action}"

        lifecycleScope.launch {
            val results = try {
                App.requestShutters("/$cmd")
            } catch (e: Throwable) {
                "[${e.message}]"
            }

            pageViewModel.sent.value = getString(R.string.sent)

            lbl_status.text = "$cmd | $results"
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
        fun newInstance(sectionNumber: Int): ActionsFragment {
            return ActionsFragment().apply {
                arguments = Bundle().apply {
                    putInt(ARG_SECTION_NUMBER, sectionNumber)
                }
            }
        }
    }
}