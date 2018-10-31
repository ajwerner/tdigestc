package com.ajwerner.tdigestc;
import static org.junit.Assert.*;
import org.junit.Test;
import com.ajwerner.tdigestc.TDigest;

public class TDigestTest {
     @Test
     public void basicallyWorks() {
          TDigest td = new TDigest(100);
          td.add(1);
          td.add(2);
          assertEquals(td.valueAt(.5), 1.5, 0);
          assertEquals(td.quantileOf(1.5), .5, 0);
     }
}

