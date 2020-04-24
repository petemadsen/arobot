package de.wiebe.arobot

import android.content.SharedPreferences
import android.os.Bundle
import android.view.MenuItem
import androidx.appcompat.app.AppCompatActivity
import androidx.preference.EditTextPreference
import androidx.preference.PreferenceFragmentCompat
import androidx.preference.PreferenceManager

class SettingsActivity : AppCompatActivity(), SharedPreferences.OnSharedPreferenceChangeListener {
    private val settingsFragment = SettingsFragment()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_settings)

        setSupportActionBar(findViewById(R.id.my_toolbar))
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        supportFragmentManager
                .beginTransaction()
                .replace(R.id.settings, settingsFragment)
                .commit()

        val prefs: SharedPreferences = PreferenceManager.getDefaultSharedPreferences(this)
        onSharedPreferenceChanged(prefs, SHUTTERS_ADDRESS)
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        if (item.itemId == android.R.id.home) {
            finish()
            return true
        }
        println("--sth--${item.itemId}")
        return super.onOptionsItemSelected(item)
    }

    override fun onSharedPreferenceChanged(sharedPreferences: SharedPreferences?, key: String?) {
        val pref = settingsFragment.findPreference<EditTextPreference>(SHUTTERS_ADDRESS)
        println("--pref" + pref.toString())
        /*
        val pref: Preference? = settingsFragment.findPreference(key) as Preference
        if (pref is EditTextPreference) {
            pref.summary = pref.text
        }
         */
    }


    class SettingsFragment : PreferenceFragmentCompat() {
        override fun onCreatePreferences(savedInstanceState: Bundle?, rootKey: String?) {
            addPreferencesFromResource(R.xml.preferences)
//            setPreferencesFromResource(R.xml.preferences, rootKey)
        }
    }
}
