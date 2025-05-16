import qrcode

img = qrcode.make("https://gitee.com/wzz6423")
img.save("gitee.png")

img = qrcode.make("https://github.com/wzz6423")
img.save("github.png")

img = qrcode.make("https://blog.csdn.net/wzzzz6423")
img.save("CSDN.png")

img = qrcode.make("https://gitee.com/wzz6423/my-code/tree/master/project/NexusLogger")
img.save("Log.png")