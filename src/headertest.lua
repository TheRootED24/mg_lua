mg = require "mg_lua"
hdr = mg.http.header.newt()

hdr.name = "test"
hdr.value = "header1"
--hdr2 = mg.http.header.newt("testy", "me")
print(hdr.value)
hdr2 = mg.http.header.newt("testy", "me")
ctx = hdr2.ctx
print(hdr2.name)
print(hdr2.value)
hdr3 = mg.http.header.newt(hdr);

print(hdr3.name)

print(hdr2.name:len())