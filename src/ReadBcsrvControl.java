import java.util.ArrayList;
import com.maxeler.maxcompiler.v2.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v2.kernelcompiler.KernelParameters;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.memory.*;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.composite.*;

class ReadBcsrvControl extends Kernel {

    protected ReadBcsrvControl(KernelParameters parameters,
                               SpmvEngineParams engineParams,
                               int numPipes) {
        super(parameters);

        DFEVar compressionEnabled = io.scalarInput("compression_enabled", dfeUInt(32)).eq(1);

        int decodingTableEntries = 1 << engineParams.getDecodingTableBitWidth();

        // -- Read indptr values
        DFEVectorType<DFEVar> index_t =
            new DFEVectorType<DFEVar>(dfeUInt(8), numPipes);

        DFEVectorType<DFEVar> value_t =
            new DFEVectorType<DFEVar>(dfeFloat(11, 53), numPipes);

        DFEVector<DFEVar> bcsrv_indices = io.input("bcsrv_index", index_t, compressionEnabled);
        DFEVector<DFEVar> bcsrv_values  = io.input("bcsrv_values", value_t, ~compressionEnabled);

        boolean portSharing = engineParams.getEnableDecoderPortSharing();

        if (portSharing && (numPipes % 2 != 0)) {
            throw new RuntimeException("Error: When port sharing is enabled, numPipes must be even");
        }

        int numMemories = portSharing ? numPipes / 2 : numPipes;
        ArrayList<Memory<DFEVar>> memories = new ArrayList<Memory<DFEVar>>();
        for (int i = 0; i < numMemories; i++) {
            Memory<DFEVar> decoding = engineParams.getDoublePrecisionTable() ?
                mem.alloc(dfeFloat(11, 53), decodingTableEntries) :
                mem.alloc(dfeFloat(8, 24), decodingTableEntries);
            memories.add(decoding);
            decoding.mapToCPU("decoding_" + i);
        }

        for (int i = 0; i < numPipes; i++) {
            // if port sharing is enabled, each memory should have 2 reads
            // if port sharing is disabled each memory should have one read
            Memory<DFEVar> mem = portSharing? memories[i / 2] : memories[i];
            DFEVar value = compressionEnabled ?
                mem.read(bcsrv_indices[i].cast(dfeUInt(engineParams.getDecodingTableBitWidth()))).cast(dfeFloat(11, 53)) :
                bcsrv_values[i];
            io.output("rc_bcsrv_value_" + i, value, dfeFloat(11, 53));
        }
    }
}
