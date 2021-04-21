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
import SmsListener from 'react-native-android-sms-listener'
import {BACKGROUND} from './src/img';

function HomeScreen({region, setRegion, marker, setMarker, user, setUser, charge, setCharge}) {
    const [isConnected, setIsConnected] = useState(false);   //state variable to hold boolean value of whether any device is connected
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
     if (!isConnected) {
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
        user.sendBody = "L";
        sendSMS();
    };

    const updateCharge = () => {
        user.sendBody = "C";
        sendSMS();
    };

    const updateWeight = () => {
        WriteMessage("W\r\n");
    }

    const tareWeight = () => {
        WriteMessage("T\r\n")
    }

    const updateBltCharge = () => {
        WriteMessage("B\r\n");
    }

    const sendNumber = () => {
        console.log(user.sendTo)
        WriteMessage("N" + user.sendTo + "\r\n")
    }

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
                if (coords[0] === "L") {
                    if (coords[1] === "Not Available") {
                        setMarker({...marker, fix: false});
                    }
                    else {
                        var lati = (coords[2] === "N") ? parseFloat(coords[1], 10) : 0.0 - parseFloat(coords[1], 10);
                        var longi = (coords[4] === "E") ? parseFloat(coords[3], 10) : 0.0 - parseFloat(coords[3], 10);
                        setMarker({...marker, latitude: lati, longitude: longi, fix: true});       
                    }
                }
                else if (coords[0] === "C") {
                    setCharge(parseInt(coords[1], 10))
                }
            }
        );
    }

    React.useEffect(() => {
        console.log("checking permissions");
        try {
            PermissionsAndroid.requestMultiple([PermissionsAndroid.PERMISSIONS.SEND_SMS,
                                                PermissionsAndroid.PERMISSIONS.READ_SMS,
                                                PermissionsAndroid.PERMISSIONS.RECEIVE_SMS,
                                                PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION]
                                                ).then((result) => {
                                                    if (result['android.permission.READ_SMS'] &&
                                                        result['android.permission.SEND_SMS'] &&
                                                        result['android.permission.RECEIVE_SMS'] &&
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
    }, []);

    React.useEffect(() => {
        console.log("reached here")
        BluetoothSerial.withDelimiter('\r\n').then((res) => {
            console.log("Delimiter has been set up ");
            BluetoothSerial.on('read', data => {
            setreaddata(data);
            var weight = data.split(',');
            if (weight[0] === "W") {
                setWeight(parseFloat(weight[1], 10));
            }
            else if (weight[0] === "B") {
                setCharge(parseFloat(weight[1], 10))
            }
            //console.log(`DATA FROM BLUETOOTH: ${data.data}`);
            // console.log(data.data);
            });
        })

        let smsSubcription = SmsListener.addListener(message => {
            if (message.originatingAddress == user.sendFrom) {
                console.log(message.originatingAddress)
                listSMS()
            }
        }
        );

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
                                                        width: 250, 
                                                        alignItems: 'center',
                                                        backgroundColor: '#DDD',
                                                        padding: 10,
                                                }} 
                                            onPress={checkconnection}
                                            onLongPress={BluetoothConnect}>
                            <Text>Backpack Connection</Text>
                        </TouchableOpacity>

                        {(isConnected === true) ?   <Text style={{alignSelf: 'center', padding: 10}}>Connected</Text> 
                                                :   <Text style={{alignSelf: 'center', padding: 10}}>Disconnected</Text>}
                    </View>
                    <View style={   {marginTop: 35, 
                                    flexDirection: 'row'}}>
                        <TouchableOpacity   style={{    marginLeft: 10,
                                                        width: '40%', 
                                                        alignItems: 'center',
                                                        backgroundColor: '#DDD',
                                                        padding: 10,
                                                }} 
                                            onPress={sendNumber}>
                            <Text>Send My Number</Text>
                        </TouchableOpacity>
                        <TextInput  style={{    width: '50%', 
                                                borderRadius: 20, 
                                                height: 40, 
                                                borderColor: "black", 
                                                borderWidth: 1,
                                                marginLeft: 20,
                                                color: '#000'}}
                                    onChangeText={text1 => setUser({ sendTo: text1 })}
                                    value={user.sendTo}
                                    //keyboardType={"numeric"}
                                    />
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
                                                borderColor: "black", 
                                                borderWidth: 1,
                                                marginLeft: 20,
                                                color: '#000'}}
                                    onChangeText={text2 => setUser({ sendFrom: text2 })}
                                    value={user.sendFrom}
                                    //keyboardType={"numeric"}
                                    />
                    </View>
                    <View style={   {marginTop: 35, 
                                    flexDirection: 'row',}}> 
                        <TouchableOpacity   style={{    marginLeft: 10,
                                                        width: 250, 
                                                        alignItems: 'center',
                                                        backgroundColor: '#DDD',
                                                        padding: 10
                                                }} 
                                            onPress={updateWeight}
                                            onLongPress={tareWeight}>
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
                                            onPress={updateMarker}>
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
                                            onPress={isConnected ? updateBltCharge : updateCharge}>
                            <Text>Update Battery Status</Text>
                        </TouchableOpacity>
                        <Text style={{  alignSelf: 'center', padding: 10,}}>
                            {charge}
                        </Text>
                    </View>
                </ScrollView>
            </ImageBackground>
        </View>
    );
}

function MapScreen({region, setRegion, marker, setMarker, user, setUser}) {

    return(
        <View style={{flex: 1}}>
            <MapView    style={{  height: '100%',  width: 400, alignSelf: 'center'}} 
                            region={region}
                            showsMyLocationButton={true}
                            onRegionChangeComplete={region => {setRegion(region)}}
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
        title : "BackPack",
        fix: false,
    });
    
    //GSM Part
    const [state, setState] = useState({
        sendFrom: "+17657725934",
        sendTo: "+17657018549",
        sendBody: "",
        minDate: "",
        maxDate: "",
        smsList: []
    });

    const [charge, setCharge] = useState(0);

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