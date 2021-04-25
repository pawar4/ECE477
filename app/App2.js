import * as React from 'react';
import { useState } from 'react';
import { 
    Alert, 
    Platform, 
    PermissionsAndroid, 
    StyleSheet, 
    ScrollView,
    TextInput,
    TouchableOpacity,
    Touchable,
    ImageBackground, } from 'react-native';
import { Text, View, Button } from 'react-native';
import { NavigationContainer } from '@react-navigation/native';
import { createBottomTabNavigator } from '@react-navigation/bottom-tabs';
import MapView, { Marker } from "react-native-maps";
// import BleManager from 'react-native-ble-manager';
import BluetoothSerial from 'react-native-bluetooth-serial'
import SmsAndroid from 'react-native-get-sms-android'

import {BACKGROUND} from './src/img';

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
        Alert.alert('ShortPress');
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
        <View style={{height: '100%', backgroundColor: '#ccc'}}>
            <ImageBackground    source={BACKGROUND}
                                style={{flex: 1, width: null, height: null}}>
                <ScrollView style={{ height: '100%'}}>
                    <View style={   {marginTop: 50, 
                                    flexDirection: 'row'}}> 
                        <TouchableOpacity   style={{    marginLeft: 10,
                                                        width: 250, 
                                                        alignItems: 'center',
                                                        backgroundColor: '#DDD',
                                                        padding: 10
                                                }} 
                                            onPress={checkconnection}
                                            onLongPress={BluetoothConnect}>
                            <Text>Backpack Connection</Text>
                        </TouchableOpacity>
                        <Text style={{alignSelf: 'center', padding: 10}}>Connection Status</Text>
                    </View>
                    <View style={   {marginTop: 50, 
                                    flexDirection: 'row',}}> 
                        <TouchableOpacity   style={{    marginLeft: 10,
                                                        width: 250, 
                                                        alignItems: 'center',
                                                        backgroundColor: '#DDD',
                                                        padding: 10
                                                }} 
                                            onPress={GetWeight}
                                            onLongPress={GetWeight}>
                            <Text>Backpack Weight</Text>
                        </TouchableOpacity>
                        <Text style={{  alignSelf: 'center',
                                        padding: 10}}>{weight}</Text>
                    </View>
                </ScrollView>
            </ImageBackground>
        </View>
    );
}

function GPSScreen() {
    const [region, setRegion] = useState({
        latitude: 51.5079145,
        longitude: -0.807321,
        latitudeDelta: 0.01,
        longitudeDelta: 0.01
    });
    const [marker, setMarker] = useState({
        latitude: 50.5079145,
        longitude: -0.19,
        title : "BackPack"
    });
    
    //GSM Part
    const [state, setState] = useState({
        sendFrom: "+18455311048",
        sendBody: "",
        minDate: "",
        maxDate: "",
        smsList: []
    });

    React.useEffect(() => {
        console.log("checking SMS permissions");
        try {
            PermissionsAndroid.requestMultiple([PermissionsAndroid.PERMISSIONS.SEND_SMS,
                                                PermissionsAndroid.PERMISSIONS.READ_SMS]
                                                ).then((result) => {
                                                    if (result['android.permission.READ_SMS'] &&
                                                    result['android.permission.SEND_SMS'] === PermissionsAndroid.RESULTS.GRANTED) {
                                                        console.log("SMS Permission granted")
                                                    }
                                                    else {
                                                        console.log("SMS Permission denied")
                                                    }
                                                })
        } catch (e) {
            console.error(e);
        }
    }, []);

    const sendSMS = () => {
        console.log(state.sendFrom);
        console.log(state.sendBody);
        SmsAndroid.autoSend(
            state.sendFrom,
            state.sendBody,
            err => {
                Alert.alert("Failed to send SMS. Check console");
                console.log("SMS SEND ERROR", err);
            },
            success => {
                Alert.alert("SMS sent successfully");
            }
        );
    }

    const listSMS = (marker) => {
        const {sendFrom, minDate, maxDate} = state
        var filter = {
            box: "inbox",
            maxCount: 1,
        };
        if (minDate !== "") {
            filter.minDate = minDate
        }
        if (maxDate !== "") {
            filter.maxDate = maxDate
        }
        if (sendFrom !== "") {
            filter.address = sendFrom
        }
    
        SmsAndroid.list(
            JSON.stringify(filter),
            fail => {
                console.log("Failed with this error: " + fail);
            },
            (count, smsList) => {
                var arr = JSON.parse(smsList);
                setState({...state, smsList: arr });
                //console.log(state.smsList[0].body);
                var coords = arr[0].body.split(',');
                var lati = parseFloat(coords[0], 10);
                var longi = parseFloat(coords[1], 10);
                setMarker({...marker, latitude: lati, longitude: longi});
            }
        );
    }

    const updateMarker = () => {
        state.sendBody = "G";
        sendSMS();
    };

    return (
        <View style={{flex: 1}}> 
            <MapView    style={{  flex: 1, padding: 10}} 
                        region={region}
                        onRegionChangeComplete={region => {setRegion(region), listSMS()}}
                        showsUserLocation={true}
                        followsUserLocation={true}
                        showsMyLocationButton={true}
            >
                <Marker coordinate={marker} />
            </MapView>
            <View style={{flex: 0.1}}>
                <Button title="Update Backpack Location" onPress={() => updateMarker()}/>
                <Text style={{alignSelf: 'center', marginTop: 5}}> Last Updated Backpack Coordinates: {marker.latitude},{marker.longitude} </Text>
            </View>
        </View>
    );
}

function SettingScreen() {
    return (
        <View style={{  flex: 1,
                        justifyContent: 'center',
                        alignItems: 'center'}}>
            
            {/* <View style={{margin: 10}}>
                <Button title="Get Weight" onPress={() => reqWeight()} />
                <Text style= {{ width: '100%', borderRadius: 40, height: 40, borderColor: 'gray', borderWidth: 1, alignItems: 'center'}}>Hello</Text>
            </View>

            <View style={{margin: 10}}>
                <Button title="Get Battery Status" onPress={() => reqBattStat()} />
            </View>             */}
        </View>
    );
}

const Tab = createBottomTabNavigator();

function MyTabs() {
    return (
    <Tab.Navigator>
        <Tab.Screen name='Home' component={HomeScreen} />
        <Tab.Screen name='GPS' component={GPSScreen} />
        <Tab.Screen name="Information" component={SettingScreen} />
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