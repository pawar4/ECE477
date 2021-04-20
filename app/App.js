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

function HomeScreen({region, setRegion, marker, setMarker, user, setUser, charge, setCharge}) {
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

   const sendSMS = () => {
        console.log(user.sendFrom);
        console.log(user.sendBody);
        SmsAndroid.autoSend(
            user.sendFrom,
            user.sendBody,
            err => {
                Alert.alert("Failed to send SMS. Check console");
                console.log("SMS SEND ERROR", err);
            },
            success => {
                Alert.alert("SMS sent successfully");
            }
        );
    }

    const updateMarker = () => {
        user.sendBody = "G";
        sendSMS();
    };

    React.useEffect(() => {
        console.log("checking permissions");
        try {
            PermissionsAndroid.requestMultiple([PermissionsAndroid.PERMISSIONS.SEND_SMS,
                                                PermissionsAndroid.PERMISSIONS.READ_SMS,
                                                PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION]
                                                ).then((result) => {
                                                    if (result['android.permission.READ_SMS'] &&
                                                        result['android.permission.SEND_SMS'] &&
                                                        result['android.permission.ACCESS_FINE_LOCATION'] 
                                                        === PermissionsAndroid.RESULTS.GRANTED) {
                                                        console.log("Permission granted")
                                                    }
                                                    else {
                                                        console.log("SMS Permission denied")
                                                    }
                                                })
        } catch (e) {
            console.error(e);
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
                <View style={{  height: 250, 
                                backgroundColor:'#fff', 
                                marginLeft: 10, 
                                marginRight: 10,
                                opacity: 0.6,
                                alignItems: 'center'
                                }}>
                    <Text style={{color: '#000', fontSize: 60}}>The</Text>
                    <Text style={{color: '#000', fontSize: 60}}>Smart</Text>
                    <Text style={{color: '#000', fontSize: 60}}>Backpack</Text>
                </View>
                <ScrollView style={{ flex: 1}}>
                <View style={   {marginTop: 35, 
                                    flexDirection: 'row'}}>
                        <TouchableOpacity   style={{    marginLeft: 10,
                                                        width: '40%', 
                                                        alignItems: 'center',
                                                        backgroundColor: '#DDD',
                                                        padding: 10,
                                                }} 
                                            onPress={checkconnection}>
                            <Text>Send My Number</Text>
                        </TouchableOpacity>
                        <TextInput  style={{    width: '50%', 
                                                borderRadius: 20, 
                                                height: 40, 
                                                borderColor: "gray", 
                                                borderWidth: 1,
                                                marginLeft: 20,
                                                color: '#fff'}}
                                    onChangeText={text => setUser({ sendTo: text })}
                                    value={user.sendFrom}
                                    keyboardType={"numeric"}/>
                    </View>
                    <View style={   {marginTop: 35, 
                                    flexDirection: 'row'}}>
                        <TouchableOpacity   style={{    marginLeft: 10,
                                                        width: '40%', 
                                                        alignItems: 'center',
                                                        backgroundColor: '#DDD',
                                                        padding: 10,
                                                }} 
                                            onPress={checkconnection}>
                            <Text>Save Backpack Number</Text>
                        </TouchableOpacity>
                        <TextInput  style={{    width: '50%', 
                                                borderRadius: 20, 
                                                height: 40, 
                                                borderColor: "gray", 
                                                borderWidth: 1,
                                                marginLeft: 20,
                                                color: '#fff'}}
                                    onChangeText={text => setUser({ sendTo: text })}
                                    value={user.sendFrom}
                                    keyboardType={"numeric"}/>
                    </View>
                    <View style={   {marginTop: 35, 
                                    flexDirection: 'row'}}> 
                        <TouchableOpacity   style={{    marginLeft: 10,
                                                        width: 250, 
                                                        alignItems: 'center',
                                                        backgroundColor: '#DDD',
                                                        padding: 10,
                                                }} 
                                            onPress={checkconnection}
                                            onLongPress={BluetoothConnect}>
                            <Text>Backpack Connection</Text>
                        </TouchableOpacity>
                        <Text style={{alignSelf: 'center', padding: 10}}>Connection Status</Text>
                    </View>
                    <View style={   {marginTop: 35, 
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
                    <View style={   {marginTop: 35, 
                                    flexDirection: 'row',}}> 
                        <TouchableOpacity   style={{    marginLeft: 10,
                                                        width: 250, 
                                                        alignItems: 'center',
                                                        backgroundColor: '#DDD',
                                                        padding: 10
                                                }} 
                                            onPress={GetWeight}
                                            onLongPress={GetWeight}>
                            <Text>Update Location</Text>
                        </TouchableOpacity>
                        <Text style={{  alignSelf: 'center', padding: 5}}>
                            {marker.latitude},{marker.longitude}
                        </Text>
                    </View>
                    <View style={   {marginTop: 35, 
                                    flexDirection: 'row',}}> 
                        <TouchableOpacity   style={{    marginLeft: 10,
                                                        width: 250, 
                                                        alignItems: 'center',
                                                        backgroundColor: '#DDD',
                                                        padding: 10
                                                }} 
                                            onPress={GetWeight}
                                            onLongPress={GetWeight}>
                            <Text>Update Battery Status</Text>
                        </TouchableOpacity>
                        <Text style={{  alignSelf: 'center', padding: 5}}>
                            70%
                        </Text>
                    </View>
                </ScrollView>
            </ImageBackground>
        </View>
    );
}

function MapScreen({region, setRegion, marker, setMarker, user, setUser}) {
    
    const listSMS = (marker) => {
        const {sendFrom, minDate, maxDate} = user
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
                setUser({...user, smsList: arr });
                //console.log(user.smsList[0].body);
                var coords = arr[0].body.split(',');
                var lati = parseFloat(coords[0], 10);
                var longi = parseFloat(coords[1], 10);
                setMarker({...marker, latitude: lati, longitude: longi});
            }
        );
    }

    return(
        <View style={{flex: 1}}>
            <MapView    style={{  height: '100%',  width: 400, alignSelf: 'center'}} 
                            region={region}
                            showsMyLocationButton={true}
                            onRegionChangeComplete={region => {setRegion(region), listSMS()}}
                            showsUserLocation={true}
                            followsUserLocation={true}
                >
                    <Marker coordinate={marker} />
                </MapView>
        </View>


    );
}

const Tab = createBottomTabNavigator();

function MyTabs() {
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

    const [charge, setCharge] = useState(-1);

    return (
    <Tab.Navigator>
        <Tab.Screen name='Home'>
            {() => <HomeScreen  region={region} 
                                setRegion={setRegion}
                                marker={marker}
                                setMarker={setMarker}
                                user={state}
                                setUser={setState}
                                charge={charge}
                                setCharge={setCharge}
                    />
            }
        </Tab.Screen>
        <Tab.Screen name='Map'>
            {() => <MapScreen   region={region}
                                setRegion={setRegion}
                                marker={marker}
                                setMarker={setMarker}
                                user={state}
                                setUser={setState}
                    />
            }
        </Tab.Screen>
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