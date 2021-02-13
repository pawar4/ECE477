import * as React from 'react';
import { useState } from 'react';
import { Text, View } from 'react-native';
import { NavigationContainer } from '@react-navigation/native';
import { createBottomTabNavigator } from '@react-navigation/bottom-tabs';
import MapView from "react-native-maps";

function HomeScreen() {
    return (
        <View style={{  flex: 1,
                        justifyContent: 'center',
                        alignItems: 'center'}}>
            <Text>Home</Text>
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
        <MapView 
            style={{  flex: 1  }} 
            region={region}
            onRegionChangeComplete={region => setRegion(region)}
            showsUserLocation={true}
            followsUserLocation={true}
        />
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

// const App = () => {
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
//         />
//     );
// }

// export default App;