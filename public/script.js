const firebaseConfig = {
   apiKey: "AIzaSyCy5xj93dr2CIDGJZBLt_88kLlAbFS4UUg",
   authDomain: "mini-iot-1.firebaseapp.com",
   databaseURL: "https://mini-iot-1-default-rtdb.firebaseio.com",
   projectId: "mini-iot-1",
   storageBucket: "mini-iot-1.firebasestorage.app",
   messagingSenderId: "421931670164",
   appId: "1:421931670164:web:a85b1c3c8a4ed014457281",
   measurementId: "G-YY4HT28KFH",
};

firebase.initializeApp(firebaseConfig);
const database = firebase.database();

const tempDisplay = document.getElementById("temperature");
const alertBox = document.getElementById("alert");
const lightStatusDisplay = document.getElementById("light-status");

database.ref("device/sensor/currentTemp").on("value", (snapshot) => {
   const temperature = snapshot.val();
   tempDisplay.textContent = temperature + " °C";
   console.log("Temperature:", temperature);

   if (temperature > 80) {
      tempDisplay.style.color = "red";
      alertBox.textContent = "⚠️ อุณหภูมิสูง! ไฟถูกปิด!";
      database.ref("/alert").set(true);
   } else {
      tempDisplay.style.color = "#ffcc00";
      alertBox.textContent = "อุณหภูมิอยู่ในระดับปกติ";
      database.ref("/alert").set(false);
   }
});

database.ref("device/sensor/applianceState").on("value", (snapshot) => {
   const lightState = snapshot.val();
   if (lightState === 1) {
      lightStatusDisplay.textContent = "Light is ON";
      lightStatusDisplay.style.color = "green";
   } else if (lightState === 0) {
      lightStatusDisplay.textContent = "Light is OFF";
      lightStatusDisplay.style.color = "red";
   } else {
      lightStatusDisplay.textContent = "Unknown";
      lightStatusDisplay.style.color = "gray";
   }
});

const offButton = document.querySelector(".btn.off");
const onButton = document.querySelector(".btn.on");

offButton.addEventListener("click", () => {
   database.ref("device/website/control").set(0);
   console.log("Appliance state set to OFF");
});

onButton.addEventListener("click", () => {
   database.ref("device/website/control").set(1);
   console.log("Appliance state set to ON");
});
