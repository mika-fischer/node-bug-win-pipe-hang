import { spawn } from "node:child_process";

const child = spawn("child.exe", { stdio: [0, 1, 2, "pipe", "pipe"] });
const in_pipe = child.stdio[3];
const out_pipe = child.stdio[4];

function write_req() {
  out_pipe.write(Buffer.alloc(1, "X"));
  console.log(`P: req`);
}

in_pipe.on("readable", () => {
  const res = in_pipe.read();
  if (res) {
    console.log(`P: res: ${res.byteLength}`);
    setImmediate(write_req); // setImmediate is important!
  }
});

write_req();
