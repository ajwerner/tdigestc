package com.ajwerner.tdigestc;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;
import java.io.File;
import java.io.IOException;
import java.nio.file.Files;

public class TDigest {
     private long ptr;

     public TDigest(int size) {
          this.ptr = td_new(size);
     }
     
     public void add(double val) {
          this.add(val, 1);
     }

     public void add(double val, double count) {
          td_add(this.ptr, val, count);
     }
     
     public double valueAt(double q) {
          return td_value_at(this.ptr, q);
     }
     
     public double quantileOf(double val) {
          return td_quantile_of(this.ptr, val);
     }

     @Override
     public void finalize() {
          td_free(this.ptr);
     }

     private native long td_new(int size);
     private native void td_add(long ptr, double val, double count);
     private native double td_value_at(long ptr, double q);
     private native void td_free(long ptr);
     private native double td_quantile_of(long ptr, double val);
     
     static {
          InputStream is = TDigest.class.getResourceAsStream("/TDigest.so");
          try {
               File file = File.createTempFile("lib", ".so");
               OutputStream os = new FileOutputStream(file);
               byte[] buffer = new byte[1024];
               int length;
               while ((length = is.read(buffer)) != -1) {
                    os.write(buffer, 0, length);
               }
               is.close();
               os.close();
               System.load(file.getAbsolutePath());
               file.deleteOnExit();
          } catch (IOException e) {
               System.out.println(e);
          }
          
     }

}
