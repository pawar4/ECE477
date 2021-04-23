import * as React from 'react';
import { useState } from 'react';
import { NativeEventEmitter, NativeModules, Platform, PermissionsAndroid } from 'react-native';
import { Text, View, Button } from 'react-native';
import { NavigationContainer } from '@react-navigation/native';
import { createBottomTabNavigator } from '@react-navigation/bottom-tabs';
import MapView from "react-native-maps";
// import BleManager from 'react-native-ble-manager';
import BluetoothSerial from 'react-native-bluetooth-serial'


function HomeScreen() {
    const [isconnected, setIsConnected] = useState(false);   //state variable to hold boolean value of whether any device is connected
    const [weight,setWeight] = useState("5 kg");
    const peripherals = new Map();
    const [list, setList] = useState([]);             //state variable to hold all discovered peripherals(not connected)
    const [device, setDevice] = useState(null);       // state variable to hold current connected peripheral
    const [readdata, setreaddata] = useState("");     //state variable to hold current read message

    
    //Component to discover all unpaired devices within range
    const discoverUnpaired = () => {
        BluetoothSerial.discoverUnpairedDevices()
          .then((unpairedDevices) => {
            setList(unpairedDevices);
          })
          .catch(err => {console.error(err);
            });
        for(var i = 0; i < list.length; ++i)
        {
            console.log(list[i]);
        }
    }

    discoverPaired_Connect = () => {
        discoverPaired();
        BluetoothConnect();
    }

    //Component to discover paired device (HC05) with phone
    const discoverPaired = () => {
        BluetoothSerial.list().then((pairedDevices) => {    
            setDevice(pairedDevices[0]);
            //console.log(device);
        }).catch(err => {console.error(err);
            });
    }

    //Component to connect to the HC05
    const BluetoothConnect = () => {
        BluetoothSerial.connect(device.id).then((res) => {
            console.log("Connected to device:",device.name);
            setIsConnected(true);
          }).catch(err => {console.error(err);});
        // setDevice(device);
        //BluetoothSerial.pairDevice(device);
    }

    //Component to disconnect from bluetooth device
    const BluetoothDisconnect = () => {
        BluetoothSerial.disconnect()
        .catch(err => {console.error(err);});
        setIsConnected(false);
        console.log("Disconnected from backpack")
    }

    const checkconnection = () => {
        BluetoothSerial.isConnected().then((res) =>
        {
            console.log(res)
        }).catch(err => {console.error(err);});
    }
    
    const GetWeight = () => {
        setWeight("500 kg");
    }

   const WriteMessage = (message) => {
     if (!isconnected) {
       console.log('You must connect to device first');
     }
 
     BluetoothSerial.write(message)
     .then((res) => {
       console.log('Successfuly wrote to device');
       //this.setState({ connected: true })
     })
     .catch(err => {console.error(err);});
   }

    React.useEffect(() => {
        // this is a constructor (runs once)
        if (Platform.OS === 'android' && Platform.Version >= 23) {
            PermissionsAndroid.check(PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION).then((result) => {
                if (result) {
                  console.log("Permission is OK");
                } else {
                  PermissionsAndroid.request(PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION).then((result) => {
                    if (result) {
                      console.log("User accept");
                    } else {
                      console.log("User refuse");
                    }
                  });
                }
            });
          }

          discoverPaired();       //automatically as app opens up, the app tries to connect to the backpack.

          BluetoothSerial.withDelimiter('\r\n').then((res) => {
              console.log("Delimiter has been set up ");
              BluetoothSerial.on('read', data => {
                setreaddata(data);
                //console.log(`DATA FROM BLUETOOTH: ${data.data}`);
                // console.log(data.data);
             });
          })
    }, [])

    return (
        <View style={{  flex: 1,
                        justifyContent: 'center',
                        alignItems: 'center'}}>

            <View style={{margin: 10}}>
                <Button title="Discover Paired devices" onPress={() => discoverPaired() } />
            </View>

            <View style={{margin: 10}}>
                <Button title="Connect to backpack" onPress={() => BluetoothConnect() } />
            </View>

            <View style={{margin: 10}}>
                <Button title="Disconnect from backpack" onPress={() => BluetoothDisconnect() } />
            </View>

            <View style={{margin: 10}}>
                <Button title="Check if connected" onPress={() => checkconnection() } />
            </View>

            <View style={{margin: 10}}>
                <Button title="Send request to backpack" onPress={() => WriteMessage(String("Hello\r\n")) } />
            </View>

            <View style={{margin: 10}}>
                <Button title="Get Backpack Weight" onPress={() => GetWeight() } />
            </View>

            <Text> {readdata.data} </Text>
            <Text> {weight} </Text>
        </View>
    );


}

function GPSScreen() {
    const [region, setRegion] = useState({
        latitude: 51.5079145,
        longitude: -0.0899163,
        latitudeDelta: 0.01,
        longitudeDelta: 0.01 
    });

    return (
        <View style={{flex: 1}}>
        <MapView 
            style={{  flex: 1  }} 
            region={region}
            onRegionChangeComplete={region => setRegion(region)}
            showsUserLocation={true}
            followsUserLocation={true}
        />
        </View>
    );
}

function SettingScreen() {
    return (
        <View style={{flex: 1}}>
        </View>
    );
}

const Tab = createBottomTabNavigator();

function MyTabs() {
    return (
    <Tab.Navigator>
        <Tab.Screen name='Home' component={HomeScreen} />
        <Tab.Screen name='GPS' component={GPSScreen} />
        <Tab.Screen name="Settings" component={SettingScreen} />
    </Tab.Navigator>
    );
}

export default function App() {
    return (
    <NavigationContainer>
        <MyTabs />
    </NavigationContainer>
    );
}