// import * as React from 'react';
// import { useState } from 'react';
// import { NativeEventEmitter, NativeModules, Platform, PermissionsAndroid } from 'react-native';
// import { Text, View, Button } from 'react-native';
// import { NavigationContainer } from '@react-navigation/native';
// import { createBottomTabNavigator } from '@react-navigation/bottom-tabs';
// import MapView from "react-native-maps";
// import BleManager from 'react-native-ble-manager';
// import BluetoothSerial from 'react-native-bluetooth-serial'

// const BleManagerModule = NativeModules.BleManager;
// const bleManagerEmitter = new NativeEventEmitter(BleManagerModule);

// function HomeScreen() {
//     const [isScanning, setIsScanning] = useState(false);
//     const peripherals = new Map();
//     const [list, setList] = useState([]);
    
//     const startScan = () => {
//         if (!isScanning) {
//             BleManager.scan([], 3, true).then((results) => {
//             console.log('Scanning...');
//             setIsScanning(true);
//             }).catch(err => {
//             console.error(err);
//             });
//         }    
//     }

//     const discoverUnpaired = () => {
//         BluetoothSerial.discoverUnpairedDevices()
//           .then((unpairedDevices) => {
//             // this.setState({ unpairedDevices, discovering: false });
//             setList(unpairedDevices);
//           })
//           .catch(err => {console.error(err);
//             });
//         console.log(list)
//     }

//     const handleStopScan = () => {
//         console.log('Scan is stopped');
//         setIsScanning(false);
//     }

//     const handleDiscoverPeripheral = (peripheral) => {
//         if (peripheral.name == 'LE Surface Headphones'){
//             console.log('Got ble peripheral', peripheral.id);
//             if (!peripheral.name) {
//             peripheral.name = 'NO NAME';
//             }
//             peripherals.set(peripheral.id, peripheral);
//             setList(Array.from(peripherals.values()));
//         }
//     }

//     const testPeripheral = (peripheral) => {
//         if (peripheral) {
//             if (peripheral.connected) {
//                 BleManager.disconnect(peripheral.id);
//             }
//             else {
//                 BleManager.connect(peripheral.id).then(() => {
//                     let p = peripherals.get(peripheral.id);
//                     if (p) {
//                         p.connected = true
//                         peripherals.set(peripheral.id, p);
//                         setList(Array.from(peripherals.values()));
//                     }
//                     console.log('Connected to ' + peripheral.id)
//                 })
//             }
                
//         }
//     }

//     // const BluetoothConnect = (device) => {
//     //     // this.setState({ connecting: true })
//     //     BluetoothSerial.connect(device.id).then((res) => {
//     //       console.log('Connected to device:',device.name);
//     //     //   this.setState({ device, connected: true, connecting: false });
//     //     }).catch(err => {console.error(err);});
//     // }

//     const retrieveConnected = () => {
//         BleManager.getConnectedPeripherals([]).then((results) => {
//             if (results.length == 0) {
//                 console.log('No connected peripherals')
//             }
//             console.log(results);
//             for (var i = 0; i < results.length; i++) {
//                 var peripheral = results[i];
//                 peripheral.connected = true;
//                 peripherals.set(peripheral.id, peripheral);
//                 setList(Array.from(peripherals.values()));
//             }
//         });
//     }

//     React.useEffect(() => {
//         // this is a constructor (runs once)
//         BleManager.start({ showAlert: false }).then(() => {
//         // Success code
//             console.log("Module initialized");
//         });
//         if (Platform.OS === 'android' && Platform.Version >= 23) {
//             PermissionsAndroid.check(PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION).then((result) => {
//                 if (result) {
//                   console.log("Permission is OK");
//                 } else {
//                   PermissionsAndroid.request(PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION).then((result) => {
//                     if (result) {
//                       console.log("User accept");
//                     } else {
//                       console.log("User refuse");
//                     }
//                   });
//                 }
//             });
//           }  
//         // bleManagerEmitter.addListener('BleManagerDiscoverPeripheral', handleDiscoverPeripheral);
//         // bleManagerEmitter.addListener('BleManagerStopScan', handleStopScan );
//     }, [])

//     return (
//         <View style={{  flex: 1,
//                         justifyContent: 'center',
//                         alignItems: 'center'}}>
//             <View style={{margin: 10}}>
//                 <Button 
//                     title={'Scan Bluetooth (' + (isScanning ? 'on' : 'off') + ')'}
//                     onPress={() => startScan() } 
//                 />            
//             </View>

//             <View style={{margin: 10}}>
//                 <Button title="Retrieve connected peripherals" onPress={() => retrieveConnected() } />
//             </View>

//             <View style={{margin: 10}}>
//                 <Button title="Connect to peripheral" onPress={() => testPeripheral(list[0]) } />
//             </View>

//             <View style={{margin: 10}}>
//                 <Button title="Discover unpaired devices" onPress={() => discoverUnpaired() } />
//             </View>
//         </View>
//     );
// }

// function GPSScreen() {
//     const [region, setRegion] = useState({
//         latitude: 51.5079145,
//         longitude: -0.0899163,
//         latitudeDelta: 0.01,
//         longitudeDelta: 0.01 
//     });

//     return (
//         <MapView 
//             style={{  flex: 1  }} 
//             region={region}
//             onRegionChangeComplete={region => setRegion(region)}
//             showsUserLocation={true}
//             followsUserLocation={true}
//         />
//     );
// }

// function SettingScreen() {
//     return (
//         <View style={{flex: 1}}>
//         </View>
//     );
// }

// const Tab = createBottomTabNavigator();

// function MyTabs() {
//     return (
//     <Tab.Navigator>
//         <Tab.Screen name='Home' component={HomeScreen} />
//         <Tab.Screen name='GPS' component={GPSScreen} />
//         <Tab.Screen name="Settings" component={SettingScreen} />
//     </Tab.Navigator>
//     );
// }

// export default function App() {


//     return (
//     <NavigationContainer>
//         <MyTabs />
//     </NavigationContainer>
//     );
// }