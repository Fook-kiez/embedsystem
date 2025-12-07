const express = require("express");
const cors = require("cors");

const app = express();
const PORT = 3000;
const GS_SCRIPT_URL = "https://script.google.com/macros/s/AKfycbxO-ctav5gHeIAKM6ECD0Vbg9Nj44wY26-HohVQs4ysLqd3h0-OV2QaeII_iInqOLg/exec";
// allow your website to fetch
app.use(cors());
app.use(express.json());
let lightstate = 0 ;

let textData = {"message": "Hello from Python!"};
let latestData = {
  "temperature": 26.75,
  "humidity": 58.20,
  "have_fire": 0,
  "light_level": 1.65,
  "temperature2": 27.10
}; // in-memory storage for demo
async function sendToGoogleSheets(payload) {
  try {
    const params = new URLSearchParams(payload);
    const url = `${GS_SCRIPT_URL}?${params.toString()}`;

    const resp = await fetch(url);
    const text = await resp.text();
    //console.log("Google Sheets response:", text);
  } catch (err) {
    console.error("Error sending to Google Sheets:", err.message);
  }
}
async function sendToThingSpeak(data) {
  const apiKey = "9MKQ2GA3BRGWGL5W";
  
  const url = `https://api.thingspeak.com/update?api_key=${apiKey}`
    + `&field1=${data.temperature}`
    + `&field2=${data.humidity}`
    + `&field3=${data.have_fire}`
    + `&field4=${data.light_level}`
    + `&field5=${data.temperature2}`
    + `&field6=${data.lightstate}`;

  const resp = await fetch(url);
  const text = await resp.text();
  console.log("Thingspeak Response:", text);
}
// endpoint that ESP32 will POST to
app.post("/api/data", async (req, res) => {
  console.log("Received data:", req.body);
  latestData = {
    ...req.body,
    timestamp: new Date().toISOString(),
  };
  res.json({ status: "ok" });
  await sendToGoogleSheets({
    temperature: latestData.temperature,
    humidity: latestData.humidity,
    have_fire: latestData.have_fire,
    light_level: latestData.light_level,
    temperature2: latestData.temperature2,
    lightstate: lightstate,           // also log current lightstate
  });
  await sendToThingSpeak(latestData);
});
app.post("/api/datatext", (req, res) => {
  //console.log("Received data:", req.body);
  textData = {
    ...req.body,
    timestamp: new Date().toISOString(),
  };
  res.json({ status: "ok" });
});
app.get("/api/data/text", (req, res) => {
  if (!textData) {
    return res.status(404).json({ error: "No data yet" });
  }
  res.json(textData);
});
// endpoint that your web page can GET
app.get("/api/data/latest", (req, res) => {
  if (!latestData) {
    return res.status(404).json({ error: "No data yet" });
  }
  res.json(latestData);
});
app.get("/api/light", (req, res) => {
  if (lightstate==0) {
    lightstate = 1;
  }
  else {
    lightstate = 0;
  }
  return res.status(200).json({ status: "ok" });
});
app.get("/api/data/light", (req, res) => {
  if (lightstate==null) {
    return res.status(404).json({ error: "No data yet" });
  }
  res.json(lightstate);
});
app.get("/api/data/fire", (req, res) => {
  fire = latestData.have_fire;
  if (fire==null) {
    return res.status(404).json({ error: "No data yet" });
  }
  res.json(fire);
});

app.listen(PORT, () => {
  console.log(`Server running on http://0.0.0.0:${PORT}`);
});
