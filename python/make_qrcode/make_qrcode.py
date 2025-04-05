import qrcode

img = qrcode.make("https://gitee.com/wzz6423")
img.save("gitee.png")

img = qrcode.make("https://github.com/wzz6423")
img.save("github.png")

img = qrcode.make("https://blog.csdn.net/wzzzz6423")
img.save("CADN.png")