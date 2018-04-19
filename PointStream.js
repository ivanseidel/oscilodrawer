module.exports = class PointStream {

  constructor(file) {
    this.index = 0
    this.frames = require(file)
  }

  next() {
    let frame = this.frames[this.index]
    this.index++

    // Reset frame when reaches end
    if (this.index >= this.frames.length) {
      this.index = 0
    }

    return frame
  }

  nextBuffer() {
    let array = this.next()
    
    if (array.length > 900) {
      throw new Error('Tamanho máximo excedido: ' + 900)
    }

    let len = array.length
    let payload = new Buffer.alloc(array.length)

    for (let i = 0; i < len; i++) {
      payload.writeUInt8(array[i], i)
    }

    return payload
  }
}