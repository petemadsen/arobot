package de.wiebe.arobot

import android.Manifest
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Bundle
import android.provider.ContactsContract
import android.view.Menu
import android.view.MenuItem
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.Observer
import androidx.lifecycle.ViewModelProviders
import androidx.viewpager.widget.ViewPager
import com.google.android.material.floatingactionbutton.FloatingActionButton
import com.google.android.material.snackbar.Snackbar
import com.google.android.material.tabs.TabLayout
import de.wiebe.arobot.ui.main.PageViewModel
import de.wiebe.arobot.ui.main.SectionsPagerAdapter
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {
    private lateinit var pageViewModel: PageViewModel

    companion object {
        const val PERMISSIONS_REQUEST_READ_CONTACTS = 100
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val sectionsPagerAdapter = SectionsPagerAdapter(this, supportFragmentManager)
        val viewPager: ViewPager = findViewById(R.id.view_pager)
        viewPager.adapter = sectionsPagerAdapter
        viewPager.offscreenPageLimit = sectionsPagerAdapter.count

        val tabs: TabLayout = findViewById(R.id.tabs)
        tabs.setupWithViewPager(viewPager)

        val fab: FloatingActionButton = findViewById(R.id.fab)
        fab.setOnClickListener { view ->
            Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG).show()
        }

        pageViewModel = ViewModelProviders.of(this)[PageViewModel::class.java]
        pageViewModel.sent.observe(this, Observer<String> {message ->
            Snackbar.make(viewPager, message, Snackbar.LENGTH_LONG).show()
        })
        pageViewModel.connected.observe(this, Observer<Boolean> {connected ->
            val msg = if (connected) getString(R.string.connected) else getString(R.string.no_connection)
            Snackbar.make(view_pager, msg, Snackbar.LENGTH_LONG).show()
        })

        setSupportActionBar(findViewById(R.id.my_toolbar))

        if (checkSelfPermission(Manifest.permission.READ_CONTACTS) != PackageManager.PERMISSION_GRANTED) {
            println("--NEED CONTACTS")
            requestPermissions(arrayOf(Manifest.permission.READ_CONTACTS), PERMISSIONS_REQUEST_READ_CONTACTS)
        } else {
            readContacts()
        }
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        menuInflater.inflate(R.menu.menu_main, menu)
        pageViewModel.connect()
        return super.onCreateOptionsMenu(menu)
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        if (item.itemId == R.id.action_settings) {
            val intent = Intent(this, SettingsActivity::class.java)
            startActivity(intent)
            return true
        } else if (item.itemId == R.id.action_sync) {
            pageViewModel.connect()
            return true
        }

        return super.onOptionsItemSelected(item)
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        if (requestCode == PERMISSIONS_REQUEST_READ_CONTACTS) {
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                readContacts()
            }
        }
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
    }

    private fun readContacts() {
        val contacts = mutableMapOf<String, String>()

        val c = contentResolver.query(ContactsContract.Contacts.CONTENT_URI, null, null, null, null)
        println("--c $c")
        c?.let {
            println("--c #${c.count}")
            while (c.count != 0 && c.moveToNext()) {
                val id = c.getString(c.getColumnIndex(ContactsContract.Contacts._ID))
                val name = c.getString(c.getColumnIndex(ContactsContract.Contacts.DISPLAY_NAME))
                val hasNumber = c.getString(c.getColumnIndex(ContactsContract.Contacts.HAS_PHONE_NUMBER)).toInt()

                if (hasNumber == 1) {
                    val cp = contentResolver.query(ContactsContract.CommonDataKinds.Phone.CONTENT_URI, null,
                        ContactsContract.CommonDataKinds.Phone.CONTACT_ID + "=?", arrayOf(id), null)
                    cp?.let {
                        while (cp.count != 0 && cp.moveToNext()) {
                            val number = cp.getString(cp.getColumnIndex(ContactsContract.CommonDataKinds.Phone.NUMBER))
                            val n = number.filter { it >= '0' && it <= '9' }
                            println("--CONTACT: $name -> $number ($n)")
                            contacts[n] = name
                        }
                        cp.close()
                    }
                }
            }
            c.close()
        }

        for ((number, name) in contacts) {
            println("--CON/$number/$name")
        }
        pageViewModel.phoneContacts.value = contacts
    }
}
