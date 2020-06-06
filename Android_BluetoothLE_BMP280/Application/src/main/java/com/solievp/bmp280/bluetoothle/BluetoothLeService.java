/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.solievp.bmp280.bluetoothle;
/*Импорт классов*/
import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

import java.util.List;
import java.util.UUID;

/**
 * Служба для управления соединения и передачи данных с сервера GATT, размещенного на
 *данном устройстве Bluetooth LE.
 */
public class BluetoothLeService extends Service {
    private final static String TAG = BluetoothLeService.class.getSimpleName();

    private BluetoothManager mBluetoothManager;
    private BluetoothAdapter mBluetoothAdapter;
    private String mBluetoothDeviceAddress;
    private BluetoothGatt mBluetoothGatt;
    private int mConnectionState = STATE_DISCONNECTED;

    private static final int STATE_DISCONNECTED = 0;
    private static final int STATE_CONNECTING = 1;
    private static final int STATE_CONNECTED = 2;

    public final static String ACTION_GATT_CONNECTED =
            "com.example.bluetooth.le.ACTION_GATT_CONNECTED";
    public final static String ACTION_GATT_DISCONNECTED =
            "com.example.bluetooth.le.ACTION_GATT_DISCONNECTED";
    public final static String ACTION_GATT_SERVICES_DISCOVERED =
            "com.example.bluetooth.le.ACTION_GATT_SERVICES_DISCOVERED";
    public final static String ACTION_DATA_AVAILABLE =
            "com.example.bluetooth.le.ACTION_DATA_AVAILABLE";
    public final static String EXTRA_DATA =
            "com.example.bluetooth.le.EXTRA_DATA";

    public final static UUID UUID_HEART_RATE_MEASUREMENT =
            UUID.fromString(SampleGattAttributes.HEART_RATE_MEASUREMENT);
    //Реализует методы обратного вызова для событий GATT, которые заботятся о приложении.
    // Например, изменение соединения и обнаружение сервисов.
    private final BluetoothGattCallback mGattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            String intentAction;
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                intentAction = ACTION_GATT_CONNECTED;
                mConnectionState = STATE_CONNECTED;
                broadcastUpdate(intentAction);
                Log.i(TAG, "Подключен в GATT сервер");
                // Попуытка обнаружения GATT профиля после успешного подключения
                Log.i(TAG, "Попытка запустить обнаруженные службы:" +
                        mBluetoothGatt.discoverServices());

            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                intentAction = ACTION_GATT_DISCONNECTED;
                mConnectionState = STATE_DISCONNECTED;
                Log.i(TAG, "Отключен от GATT сервера.");
                broadcastUpdate(intentAction);
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                broadcastUpdate(ACTION_GATT_SERVICES_DISCOVERED);
            } else {
                Log.w(TAG, "Получены данные об обнаруженных услугах:: " + status);
            }
        }

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt,
                                         BluetoothGattCharacteristic characteristic,
                                         int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic);
            }
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt,
                                            BluetoothGattCharacteristic characteristic) {
            broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic);
        }
    };

    private void broadcastUpdate(final String action) {
        final Intent intent = new Intent(action);
        sendBroadcast(intent);
    }

    private void broadcastUpdate(final String action,
                                 final BluetoothGattCharacteristic characteristic) {
        final Intent intent = new Intent(action);

        // Это специальная обработка для профиля измерения пульса. Разбор данных выполняется
        // согласно спецификациям профиля:
        // http://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u
        // =org.bluetooth.characteristic.heart_rate_measurement.xml
        if (UUID_HEART_RATE_MEASUREMENT.equals(characteristic.getUuid())) {
            int flag = characteristic.getProperties();
            int format = -1;
            if ((flag & 0x01) != 0) {
                format = BluetoothGattCharacteristic.FORMAT_UINT16;
                Log.d(TAG, "Формат данных измерения пульса UINT16.");
            } else {
                format = BluetoothGattCharacteristic.FORMAT_UINT8;
                Log.d(TAG, "Формат данных измерения пульса UINT8.");
            }
            final int heartRate = characteristic.getIntValue(format, 1);
            Log.d(TAG, String.format("Полученное значение измерения пульса: %d", heartRate));
            intent.putExtra(EXTRA_DATA, String.valueOf(heartRate));
        } else {
            // Для остальных профилей, которые пишут данные в HEX.
            final byte[] data = characteristic.getValue();
            if (data != null && data.length > 0) {
                /*Конечная StringBuilder stringBuilder = new StringBuilder(data.length);
                for(byte byteChar : data)
                    stringBuilder.append(String.format("%02X ", byteChar));
                intent.putExtra(EXTRA_DATA, new String(data) + "\n" + stringBuilder.toString());*/
                intent.putExtra(EXTRA_DATA, new String(data));
            }
        }
        sendBroadcast(intent);
    }

    public class LocalBinder extends Binder {
        BluetoothLeService getService() {
            return BluetoothLeService.this;
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    @Override
    public boolean onUnbind(Intent intent) {
        //После использования данного устройства вы должны убедиться, что BluetoothGatt.close
        // () вызывается таким образом, чтобы ресурсы были очищены. В этом конкретном примере
        // close () вызывается, когда пользовательский интерфейс отключен от службы.
        close();
        return super.onUnbind(intent);
    }

    private final IBinder mBinder = new LocalBinder();

    /**
     * Инициализация ссылки на локальный адаптер Bluetooth.
     *
     * @return Вернет TRUE если инициализация прошла успешно
     */
    public boolean initialize() {
        // Для API уровня 18 и выше, получите разрешение BluetoothAdapter через BluetoothManager.
        if (mBluetoothManager == null) {
            mBluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
            if (mBluetoothManager == null) {
                Log.e(TAG, "Невозможно инициализировать BluetoothManager...");
                return false;
            }
        }

        mBluetoothAdapter = mBluetoothManager.getAdapter();
        if (mBluetoothAdapter == null) {
            Log.e(TAG, "Невозможно получить доступ в Bluetooth  адаптер.");
            return false;
        }

        return true;
    }

    /**
     * Подключение к серверу GATT, расположенному на устройстве Bluetooth LE.
     *
     * @param address Адрес устройства получателя.
     *
     * @return Вернет true, если соединение успешно установлено. Результат подключения
     *          сообщается асинхронно через обратный вызов
     *         {@code BluetoothGattCallback#onConnectionStateChange(android.bluetooth.BluetoothGatt, int, int)}
     */
    public boolean connect(final String address) {
        if (mBluetoothAdapter == null || address == null) {
            Log.w(TAG, "BluetoothAdapter не инициализирован или не верно указан адрес.");
            return false;
        }

        // Раннее подключенные устройства, попытка соединения
        if (mBluetoothDeviceAddress != null && address.equals(mBluetoothDeviceAddress)
                && mBluetoothGatt != null) {
            Log.d(TAG, "Происходит попытка подключения существующего mBluetoothGatt .");
            if (mBluetoothGatt.connect()) {
                mConnectionState = STATE_CONNECTING;
                return true;
            } else {
                return false;
            }
        }

        final BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
        if (device == null) {
            Log.w(TAG, "Устройство не найдено. Соединение невозможно");
            return false;
        }
        // Из за нужды прямого подключения к устройству, на необходимо чтобы параметр вернуло "ложь"
        mBluetoothGatt = device.connectGatt(this, false, mGattCallback);
        Log.d(TAG, "Попытка создания нового подключения.");
        mBluetoothDeviceAddress = address;
        mConnectionState = STATE_CONNECTING;
        return true;
    }

    /**Разъединяет существующее соединение или отменяет ожидающее соединение. Результат отключения
     *сообщается асинхронно через обратный вызов
     * {@code BluetoothGattCallback#onConnectionStateChange(android.bluetooth.BluetoothGatt, int, int)}
     */
    public void disconnect() {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter не инициализирован");
            return;
        }
        mBluetoothGatt.disconnect();
    }

    /**
     * После подключения к устройству BLEприложение должно вызвать, этот метод для упбеждения правильности
     * выпущенных ресурсов
     */
    public void close() {
        if (mBluetoothGatt == null) {
            return;
        }
        mBluetoothGatt.close();
        mBluetoothGatt = null;
    }

    /**
     * Запрос чтения по данному коду{@code BluetoothGattCharacteristic}. Выводиться результат чтения
     * асинхронно сообщается через обратный вызов
     * {@code BluetoothGattCallback#onCharacteristicRead(android.bluetooth.BluetoothGatt, android.bluetooth.BluetoothGattCharacteristic, int)}
     * @param characteristic Характеристики для чтения из.
     */
    public void readCharacteristic(BluetoothGattCharacteristic characteristic) {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter не инициализирован");
            return;
        }
        mBluetoothGatt.readCharacteristic(characteristic);
    }

    /**
     * Включает или отключает уведомление о заданной характеристике.
     * @param characteristic Характеристика действия на...
     * @param enabled Если true, включите уведомление иначе если ложно.
     */
    public void setCharacteristicNotification(BluetoothGattCharacteristic characteristic,
                                              boolean enabled) {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter не ицициализирован");
            return;
        }
        mBluetoothGatt.setCharacteristicNotification(characteristic, enabled);

        // Спецификации для измерения импульса .
        if (UUID_HEART_RATE_MEASUREMENT.equals(characteristic.getUuid())) {
            BluetoothGattDescriptor descriptor = characteristic.getDescriptor(
                    UUID.fromString(SampleGattAttributes.CLIENT_CHARACTERISTIC_CONFIG));
            descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
            mBluetoothGatt.writeDescriptor(descriptor);
        }
    }

    /**
     * Получает список поддерживаемых сервисов GATT на подключенном устройстве.
     * Это должно бытьвызыватся только после успешного завершения{@code BluetoothGatt#discoverServices()} .
     *
     * @return  {@code List} поддерживаемых устройств.
     */
    public List<BluetoothGattService> getSupportedGattServices() {
        if (mBluetoothGatt == null) return null;

        return mBluetoothGatt.getServices();
    }
/**GATT профиль для подклчюения к HM-10**/
    public void writeCharacteristic(String input)
    {
        try
        {
            BluetoothGattService Service = mBluetoothGatt.getService(UUID.fromString("00001800-0000-1000-8000-00805f9b34fb"));
            BluetoothGattCharacteristic charac = Service.getCharacteristic(UUID.fromString("00001801-0000-1000-8000-00805f9b34fb"));
            charac.setValue(input);
            mBluetoothGatt.writeCharacteristic(charac);
        }catch (IllegalStateException | NullPointerException e)
        {
            Log.e(TAG, "Устройство которое имеет другую характеристику чем 00001800-0000-1000-8000-00805f9b34fb");
        }
    }
}
