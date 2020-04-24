package de.wiebe.shutters.ui.main

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.EditText
import androidx.appcompat.app.AlertDialog
import androidx.fragment.app.Fragment
import androidx.lifecycle.Observer
import androidx.lifecycle.ViewModelProviders
import androidx.recyclerview.widget.LinearLayoutManager
import de.wiebe.shutters.R
import kotlinx.android.synthetic.main.fragment_phonebook.*


class PhonebookFragment : Fragment() {
    private lateinit var pageViewModel: PageViewModel
    private lateinit var linearLayoutManager: LinearLayoutManager
    private lateinit var contactAdapter: PhoneBookAdapter

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        pageViewModel = ViewModelProviders.of(requireActivity()).get(PageViewModel::class.java)
    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        println("--phonebook-fragment-create")
        return inflater.inflate(R.layout.fragment_phonebook, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        linearLayoutManager = LinearLayoutManager(context)
        numbers.layoutManager = linearLayoutManager
    }

    override fun onActivityCreated(savedInstanceState: Bundle?) {
        super.onActivityCreated(savedInstanceState)

        with (pageViewModel) {
            editContact.observe(viewLifecycleOwner, Observer<Contact> { editContact(it) })
            delContact.observe(viewLifecycleOwner, Observer<Contact> { delContact(it) })
            connected.observe(viewLifecycleOwner, Observer<Boolean> { if (it) { pageViewModel.getContacts() } })
            contacts.observe(viewLifecycleOwner, Observer<List<Contact>> {list ->
                println("--PHONEBOOK/new-contacts/#${list.size}")
                contactAdapter = PhoneBookAdapter(list, pageViewModel)
                numbers.adapter = contactAdapter

                // FIXME: use DiffUtil
//                contactAdapter.setContacts(list)
            })
        }
    }

    private fun delContact(contact: Contact) {
        val dlg = AlertDialog.Builder(requireContext())
        dlg.setIcon(R.mipmap.ic_launcher_round) //set alertdialog icon
//        dlg.setTitle("Title!") //set alertdialog title
        dlg.setMessage(R.string.ask_delete)
        dlg.setPositiveButton(R.string.yes) { dialog, _ ->
            pageViewModel.deleteContact(contact)
            pageViewModel.getContacts()
            dialog.dismiss()
        }
        dlg.setNegativeButton(R.string.no) { dialog, _ ->
            dialog.cancel()
        }
        dlg.show()
    }

    private fun editContact(contact: Contact) {
        println("--edit-contact: ${contact.number}")

        val dlg = AlertDialog.Builder(requireContext())
        // Get the layout inflater
        val inflater = requireActivity().layoutInflater
        val view = inflater.inflate(R.layout.dialog_edit_contact, null)

        view.findViewById<EditText>(R.id.number).setText(contact.number)
        val editName = view.findViewById<EditText>(R.id.name)

        if (contact.name.isEmpty())
            editName.setText(contact.phoneName)
        else
            editName.setText(contact.name)

        dlg.setView(view)
            .setPositiveButton(R.string.save) { dialog, _ ->
                val name = editName.text.toString()
                if (name.isNotEmpty() && name != contact.name) {
                    pageViewModel.sendContact(Contact(contact.number, name, ""))
                    pageViewModel.getContacts()
                }
                dialog.dismiss()
            }
            .setNegativeButton(R.string.cancel) { dialog, _ ->
                dialog.cancel()
            }
        dlg.create()
        dlg.show()
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
        fun newInstance(sectionNumber: Int): PhonebookFragment {
            return PhonebookFragment().apply {
                arguments = Bundle().apply {
                    putInt(ARG_SECTION_NUMBER, sectionNumber)
                }
            }
        }
    }
}
