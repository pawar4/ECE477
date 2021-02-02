import * as React from 'react';
import { Text, View } from 'react-native';
import { NavigationContainer } from '@react-navigation/native'
import { createBottomTabNavigator } from '@react-navigation/bottom-tabs'

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
    return (
    <View style={{ flex: 1, flexDirection: 'column'}} >
        <View style= {{ flex: 1, 
                        backgroundColor: 'powderblue', 
                        justifyContent: 'center',
                        alignItems: 'center'}}>
        <Text>Display Map and GPS here</Text>
        </View>
        <View style= {{ flex: 1, 
                        backgroundColor: 'skyblue' }}/>
    </View>
    );
}

const Tab = createBottomTabNavigator();

function MyTabs() {
    return (
    <Tab.Navigator>
        <Tab.Screen name='Home' component={HomeScreen} />
        <Tab.Screen name='GPS' component={GPSScreen} />
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