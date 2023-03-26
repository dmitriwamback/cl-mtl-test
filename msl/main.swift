import MetalKit

public class BundleClass {}

let device = MTLCreateSystemDefaultDevice()
let commandQueue = device?.makeCommandQueue()
let library      = try? device?.makeDefaultLibrary(bundle: Bundle(for: BundleClass.self))

let kernel = library?.makeFunction(name: "add")
var computePipeline: MTLComputePipelineState!

do {
    computePipeline = try device?.makeComputePipelineState(function: kernel!)
}
catch {
    print(error)
}
let elements = 10000000

func makeRandomArray() -> [Float] {

    var result: [Float] = []
    for _ in (0..<elements) {
        result.append(Float.random(in: 1..<100))
    }
    return result
}

let arr1 = makeRandomArray()
let arr2 = makeRandomArray()

let bufArr1 = device?.makeBuffer(bytes: arr1, length: MemoryLayout<Float>.size * elements, options: .storageModeShared)
let bufArr2 = device?.makeBuffer(bytes: arr2, length: MemoryLayout<Float>.size * elements, options: .storageModeShared)
let result  = device?.makeBuffer(length: elements, options: .storageModeShared)



let commandBuffer  = commandQueue?.makeCommandBuffer()
let commandEncoder = commandBuffer?.makeComputeCommandEncoder()

commandEncoder?.setComputePipelineState(computePipeline)
commandEncoder?.setBuffer(bufArr1, offset: 0, index: 0)
commandEncoder?.setBuffer(bufArr2, offset: 0, index: 1)
commandEncoder?.setBuffer(result,  offset: 0, index: 2)

var currentTime = CFAbsoluteTimeGetCurrent()

let maxThreads     = computePipeline!.maxTotalThreadsPerThreadgroup
let threadsPerGrid = MTLSize(width: elements, height: 1, depth: 1)
let threadGroup    = MTLSize(width: maxThreads, height: 1, depth: 0)

commandEncoder?.dispatchThreads(threadsPerGrid, threadsPerThreadgroup: threadGroup)
commandEncoder?.endEncoding()
commandBuffer?.commit()
commandBuffer?.waitUntilCompleted()

var resultBuffer = result?.contents().bindMemory(to: Float.self, capacity: MemoryLayout<Float>.size * nbElements)
for i in (0..<5) {
    print("\(a1[i]) + \(a2[i]) = \(Float(resultBuffer!.pointee) as Any)")
    resultBuffer = resultBuffer?.advanced(by: 1)
}

var elapsed = CFAbsoluteTimeGetCurrent() - currentTime
print("Elapsed with metal: \(elapsed) with \(elements) elements")