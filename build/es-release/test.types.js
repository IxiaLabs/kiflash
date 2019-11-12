"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const mmap_io_1 = __importDefault(require("./mmap-io"));
const fs_1 = __importDefault(require("fs"));
const fd = fs_1.default.openSync(__filename, "r+");
mmap_io_1.default.map(100, mmap_io_1.default.PROT_EXEC | mmap_io_1.default.PROT_READ, mmap_io_1.default.MAP_SHARED, fd, 0, mmap_io_1.default.MADV_SEQUENTIAL);
