package de.wiebe.arobot.ui.main

import android.view.View
import android.view.ViewGroup
import androidx.recyclerview.widget.RecyclerView
import de.wiebe.arobot.R
import kotlinx.android.synthetic.main.contact_row_item.view.*

data class Contact(val number: String, val name: String, val phoneName: String)

class PhoneBookAdapter(private val contacts: List<Contact>, private val adapter: PageViewModel) : RecyclerView.Adapter<PhoneBookAdapter.ContactHolder>() {
    override fun onBindViewHolder(holder: ContactHolder, position: Int) {
        val item = contacts[position]
        holder.bindContact(item)
    }

    override fun onCreateViewHolder(
        parent: ViewGroup,
        viewType: Int
    ): ContactHolder {
        val inflatedView = parent.inflate(R.layout.contact_row_item, false)
        return ContactHolder(inflatedView, adapter)
    }

    override fun getItemCount() = contacts.size

    class ContactHolder(v: View, a: PageViewModel) : RecyclerView.ViewHolder(v), View.OnClickListener, View.OnLongClickListener {
        private var view: View = v
        private var contact: Contact? = null
        private var adapter = a

        init {
            v.setOnClickListener(this)
            v.setOnLongClickListener(this)
        }

        override fun onClick(v: View) {
            adapter.editContact.value = contact
        }

        override fun onLongClick(v: View?): Boolean {
            if (contact!!.name.isNotEmpty()) {
                adapter.delContact.value = contact
            }
            return true
        }

        fun bindContact(contact: Contact) {
            this.contact = contact
            with (view) {
                itemImage.setImageResource(if (contact.name == "") android.R.drawable.star_off else android.R.drawable.star_on)
                itemNumber.text = contact.number
                if (contact.phoneName.isEmpty())
                    itemName.text = contact.name
                else
                    itemName.text = "${contact.name} (${contact.phoneName})"
            }
        }
    }
}
