const sleep = require('./sleep')

module.exports = class Channel {
  constructor(stream, client) {
    this.stream = stream
    this.client = client
    this.run = false
  }

  async start() {
    this.run = true

    let frames = 0
    let draft = console.draft()
    while (this.run) {
      // this.stream.nextBuffer()
      // this.stream.nextBuffer()
      // this.stream.nextBuffer()
      this.client.publish('frame', this.stream.nextBuffer())
      draft(` Streaming - Frame: ${frames++}`)
      await sleep(100)
    }
  }

  stop() {
    this.run = false
  }
}