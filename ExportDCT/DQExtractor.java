import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Platform;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import org.apache.commons.lang3.ArrayUtils;
import org.apache.commons.math3.complex.Complex;
import org.apache.commons.math3.stat.descriptive.DescriptiveStatistics;
import org.apache.commons.math3.transform.DftNormalization;

import org.apache.commons.math3.transform.FastFourierTransformer;
import org.apache.commons.math3.transform.TransformType;

import javax.imageio.ImageIO;

import static util.Util.visualizeWithJet;
import static util.Util.rem;

import util.dctCoeffExtractor;

/**
 *
 * @author markzampoglou
 */
public final class DQExtractor {

    int[][] dcts; // The array of DCT Coefficients of the image
    int maxCoeffs = 15; //How many DCT coefficients to take into account
    // The sequence of DCT coefficients: zigzag order
    int[] coeff = {1, 9, 2, 3, 10, 17, 25, 18, 11, 4, 5, 12, 19, 26, 33, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 57, 50, 43, 36, 29, 22, 15, 8, 16, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 24, 32, 39, 46, 53, 60, 61, 54, 47, 40, 48, 55, 62, 63, 56, 64};
    public double[][] probabilityMap = null;
    public BufferedImage displaySurface = null;
    public double minProbValue = Double.MAX_VALUE;
    public double maxProbValue = -Double.MAX_VALUE;

    public interface CLibrary extends Library {

        CLibrary dctLib = (CLibrary) Native.loadLibrary((Platform.isWindows() ? "ExportDCT" : "ExportDCT"), CLibrary.class);
        int testInOut(int a);
        IntByReference getDCT(String FileName);
    }

    public DQExtractor(String fileName) throws IOException {
        String imageFormat = util.Util.getImageFormat(new File(fileName));
	int[][] dcts=null;
        try {
            if (imageFormat.equalsIgnoreCase("JPEG") | imageFormat.equalsIgnoreCase("JPG")) {
                dcts = getDCTCoeffsFromFile(fileName);
            }
            else {
                System.out.println("Not a JPEG image.");
            }

        } catch (Error err) {
            err.printStackTrace();
            System.out.println("Could not load native JPEGlib-based DCT extractor, sorry.");
        }
	return dcts;
    }

    public final int[][] getDCTCoeffsFromFile(String FileName) {
	
	//Test the basic interface
        //System.out.println(CLibrary.dctLib.testInOut(3));

        IntByReference intFromCByRef;
        int[][] dctCoeffs =null;
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


        return dctCoeffs;
    }
