package de.wiebe.arobot.ui.main

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.LinearLayout
import android.widget.RadioButton
import androidx.core.view.size
import androidx.fragment.app.Fragment
import androidx.lifecycle.Observer
import androidx.lifecycle.ViewModelProviders
import de.wiebe.arobot.R
import kotlinx.android.synthetic.main.fragment_xmas.*


class XmasFragment : Fragment() {
    private lateinit var pageViewModel: PageViewModel

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        pageViewModel = ViewModelProviders.of(requireActivity()).get(PageViewModel::class.java)
    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        println("--status-fragment-create-view")
        return inflater.inflate(R.layout.fragment_xmas, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        println("--status-fragment-view-created")

        modes.setOnCheckedChangeListener { group, checkedId ->
            pageViewModel.setXmasMode((checkedId - 1).toString())
        }
    }

    override fun onActivityCreated(savedInstanceState: Bundle?) {
        super.onActivityCreated(savedInstanceState)

        pageViewModel.xmas.observe(viewLifecycleOwner, Observer<Xmas> { xmas ->
            if (xmas.numModes != -1)
                txtStatus.setText(R.string.connected)

            if (modes.size != xmas.numModes) {
                modes.removeAllViews()
                for (x in 0 until xmas.numModes) {
                    val btn = RadioButton(activity)
                    btn.text = xmas.desc[x]

                    val layout = LinearLayout.LayoutParams(
                        ViewGroup.LayoutParams.MATCH_PARENT,
                        ViewGroup.LayoutParams.WRAP_CONTENT)
                    layout.setMargins(5, 5, 5, 5)
                    btn.layoutParams = layout

                    modes.addView(btn)
                }
            }

            modes.check(xmas.mode + 1)
        })
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
        fun newInstance(sectionNumber: Int): XmasFragment {
            return XmasFragment().apply {
                arguments = Bundle().apply {
                    putInt(ARG_SECTION_NUMBER, sectionNumber)
                }
            }
        }
    }
}