const draftlog = require('draftlog').into(console).addLineListener(process.stdin)
const sleep = require('./sleep')
const path = require('path')
const mqtt = require('mqtt')

const PointStream = require('./PointStream')
const Channel = require('./Channel')

async function main() {
  var client = mqtt.connect('mqtt://localhost')

  console.log('connecting...')
  await waitConnect(client)
  console.log('connected!')

  let stream = new PointStream(path.join(__dirname, `streams/${process.env.MEDIA}.json`))
  let channel = new Channel(stream, client)
  
  await channel.start()
}

async function waitConnect(client) {
  while(!client.connected) {
    await sleep(5)
  }
}

main()

 
// client.on('connect', function () {
//   client.subscribe('display1')
//   console.log('connected!')

//   let len = 900
//   let payload = new Buffer.alloc(len)

//   for (let i = 0; i < len; i++) {
//     payload.writeUInt8((i % 256), i)
//   }

//   // let buffer = new Uint8Array(payload)

//   setInterval(() => {
//     client.publish('display1', payload)
//     console.log(`published ${len} bytes.`)
//   }, 100)
// })
 
// client.on('message', function (topic, message) {
//   // message is Buffer
//   // console.log(message.toString())
//   // client.end()
// })