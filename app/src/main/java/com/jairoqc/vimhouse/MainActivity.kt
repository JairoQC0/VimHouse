package com.jairoqc.vimhouse

import android.Manifest
import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.pm.PackageManager
import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import java.io.IOException
import java.util.*

class MainActivity : AppCompatActivity() {

    private var bluetoothAdapter: BluetoothAdapter? = null
    private var bluetoothSocket: BluetoothSocket? = null
    private var isConnected = false
    private val uuid: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")

    private lateinit var btnConnect: Button
    private lateinit var btnSend: Button
    private lateinit var txtMessage: EditText
    private lateinit var txtStatus: TextView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Inicializar vistas
        btnConnect = findViewById(R.id.btnConnect)
        btnSend = findViewById(R.id.btnSendCommand)
        txtMessage = findViewById(R.id.txtMessage)
        txtStatus = findViewById(R.id.txtStatus)

        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter()

        // Manejar botón de conectar
        btnConnect.setOnClickListener {
            if (bluetoothAdapter == null) {
                Toast.makeText(this, "El dispositivo no soporta Bluetooth", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            if (!bluetoothAdapter!!.isEnabled) {
                Toast.makeText(this, "Activa el Bluetooth primero", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }

            solicitarPermisosBluetooth()
        }

        // Manejar botón de enviar
        btnSend.setOnClickListener {
            if (isConnected) {
                val message = txtMessage.text.toString()
                sendMessage(message)
            } else {
                Toast.makeText(this, "Primero conecta un dispositivo Bluetooth", Toast.LENGTH_SHORT).show()
            }
        }
    }

    private fun solicitarPermisosBluetooth() {
        if (ActivityCompat.checkSelfPermission(
                this,
                Manifest.permission.BLUETOOTH_CONNECT
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            ActivityCompat.requestPermissions(
                this,
                arrayOf(Manifest.permission.BLUETOOTH_CONNECT),
                1
            )
            return
        }
        conectarDispositivoBluetooth()
    }

    @SuppressLint("MissingPermission")
    private fun conectarDispositivoBluetooth() {
        val pairedDevices = bluetoothAdapter?.bondedDevices
        if (pairedDevices.isNullOrEmpty()) {
            Toast.makeText(this, "No hay dispositivos emparejados", Toast.LENGTH_SHORT).show()
            return
        }

        // Busca el dispositivo con el nombre "ESP32_Alarmas"
        val device = pairedDevices.find { it.name == "ESP32_Alarmas" }
        if (device == null) {
            Toast.makeText(this, "ESP32 no encontrado. Empareja el dispositivo primero.", Toast.LENGTH_SHORT).show()
            return
        }

        txtStatus.text = "Conectando a: ${device.name}"

        try {
            val socket = device.createRfcommSocketToServiceRecord(uuid)
            socket.connect()
            bluetoothSocket = socket
            isConnected = true
            txtStatus.text = "Conectado a: ${device.name}"
        } catch (e: IOException) {
            txtStatus.text = "Error al conectar: ${e.message}"
            e.printStackTrace()
        }
    }

    private fun sendMessage(message: String) {
        if (bluetoothSocket == null) {
            Toast.makeText(this, "Socket no inicializado", Toast.LENGTH_SHORT).show()
            return
        }

        try {
            val outputStream = bluetoothSocket!!.outputStream
            outputStream.write(message.toByteArray())
            Toast.makeText(this, "Mensaje enviado: $message", Toast.LENGTH_SHORT).show()
        } catch (e: IOException) {
            Toast.makeText(this, "Error al enviar mensaje", Toast.LENGTH_SHORT).show()
            e.printStackTrace()
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        try {
            bluetoothSocket?.close()
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }
}
