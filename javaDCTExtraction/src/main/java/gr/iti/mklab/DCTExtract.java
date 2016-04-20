package gr.iti.mklab;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Platform;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;

/**
 *
 * @author markzampoglou
 * Markos Zampoglou, 2016
 * ITI-CERTH, Thessaloniki, Greece
 * http://github.com/markzampoglou/
 * http://www.linkedin.com/in/markzampoglou
 */

public class DCTExtract {

    public interface CLibrary extends Library {
        CLibrary dctLib = (CLibrary) Native.loadLibrary((Platform.isWindows() ? "ExportDCT" : "ExportDCT"), CLibrary.class);
        int testInOut(int a);
        IntByReference getDCT(String FileName);
    }

    public static int[][] getDCTCoeffsFromFile(String FileName) {
        //Test the basic interface
        //System.out.println(CLibrary.dctLib.testInOut(3));
        int[][] dctCoeffs = null;
        try {
            try {
                IntByReference intFromCByRef;
                intFromCByRef = CLibrary.dctLib.getDCT(FileName);
                Pointer p = intFromCByRef.getPointer();
                int[] imageSize = new int[2];
                p.read(0, imageSize, 0, 2);
                int nBlocksX, nBlocksY;
                nBlocksX = (int) imageSize[0];
                nBlocksY = (int) imageSize[1];
                int[] intFromC = new int[nBlocksX * nBlocksY * 64];
                p.read(8, intFromC, 0, nBlocksX * nBlocksY * 64);
                dctCoeffs = new int[(nBlocksX) * 8][(nBlocksY) * 8]; //matlab style, rows first
                int serialInd;
                for (int bxInd = 0; bxInd < nBlocksX; bxInd++) {
                    for (int byInd = 0; byInd < nBlocksY; byInd++) {
                        for (int ii = 0; ii < 8; ii++) {
                            for (int jj = 0; jj < 8; jj++) {
                                serialInd = (bxInd * (nBlocksY) + byInd) * 64 + ii * 8 + jj;
                                dctCoeffs[bxInd * 8 + ii][byInd * 8 + jj] = intFromC[serialInd];
                            }
                        }
                    }
                }
                // Catching Error is potentially dangerous practice, yet it is the only way to
                // locate problems with libraries. Be careful.
            } catch (Error err) {
                err.printStackTrace();
                System.out.println("Could not load native JPEGlib-based DCT extractor, sorry.");
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return dctCoeffs;
    }

    public static void main(String args[]){
        String imageFile;
        if (args.length==0){
            imageFile="demo.jpg";
        } else
        {
            imageFile=args[0];
        }
        int[][] yDCTs = getDCTCoeffsFromFile(imageFile);
        System.out.println("DCT array size: " + String.valueOf(yDCTs.length)+ "x" + String.valueOf(yDCTs[0].length));
        System.out.println("Eleventh block from the top, sixteenth from the left:");
        for (int ii=80;ii<87;ii++){
            for (int jj=120;jj<127;jj++){
                System.out.print(yDCTs[ii][jj]);
                System.out.print(" ");
            }
            System.out.println();
        }

    }
}
