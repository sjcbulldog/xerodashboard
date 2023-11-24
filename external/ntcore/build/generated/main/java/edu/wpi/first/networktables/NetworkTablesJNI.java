// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import edu.wpi.first.util.RuntimeLoader;
import edu.wpi.first.util.datalog.DataLog;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.EnumSet;
import java.util.OptionalLong;
import java.util.concurrent.atomic.AtomicBoolean;

public final class NetworkTablesJNI {
  static boolean libraryLoaded = false;
  static RuntimeLoader<NetworkTablesJNI> loader = null;

  public static class Helper {
    private static AtomicBoolean extractOnStaticLoad = new AtomicBoolean(true);

    public static boolean getExtractOnStaticLoad() {
      return extractOnStaticLoad.get();
    }

    public static void setExtractOnStaticLoad(boolean load) {
      extractOnStaticLoad.set(load);
    }
  }

  static {
    if (Helper.getExtractOnStaticLoad()) {
      try {
        loader =
            new RuntimeLoader<>(
                "ntcorejni", RuntimeLoader.getDefaultExtractionRoot(), NetworkTablesJNI.class);
        loader.loadLibrary();
      } catch (IOException ex) {
        ex.printStackTrace();
        System.exit(1);
      }
      libraryLoaded = true;
    }
  }

  /**
   * Force load the library.
   *
   * @throws IOException if the library fails to load
   */
  public static synchronized void forceLoad() throws IOException {
    if (libraryLoaded) {
      return;
    }
    loader =
        new RuntimeLoader<>(
            "ntcorejni", RuntimeLoader.getDefaultExtractionRoot(), NetworkTablesJNI.class);
    loader.loadLibrary();
    libraryLoaded = true;
  }

  private static PubSubOptions buildOptions(PubSubOption... options) {
    if (options.length == 0) {
      return null;  // optimize common case (JNI checks for null)
    }
    return new PubSubOptions(options);
  }

  public static native int getDefaultInstance();

  public static native int createInstance();

  public static native void destroyInstance(int inst);

  public static native int getInstanceFromHandle(int handle);

  private static native int getEntryImpl(
      int topic, int type, String typeStr, PubSubOptions options);

  public static native int getEntry(int inst, String key);

  public static int getEntry(
      int topic, int type, String typeStr, PubSubOptions options) {
    return getEntryImpl(topic, type, typeStr, options);
  }

  public static int getEntry(
      int topic, int type, String typeStr, PubSubOption... options) {
    return getEntryImpl(topic, type, typeStr, buildOptions(options));
  }

  public static native String getEntryName(int entry);

  public static native long getEntryLastChange(int entry);

  public static native int getType(int entry);

  /* Topic functions */

  public static native int[] getTopics(int inst, String prefix, int types);

  public static native int[] getTopicsStr(int inst, String prefix, String[] types);

  public static native TopicInfo[] getTopicInfos(
      NetworkTableInstance instObject, int inst, String prefix, int types);

  public static native TopicInfo[] getTopicInfosStr(
      NetworkTableInstance instObject, int inst, String prefix, String[] types);

  public static native int getTopic(int inst, String name);

  public static native String getTopicName(int topic);

  public static native int getTopicType(int topic);

  public static native void setTopicPersistent(int topic, boolean value);

  public static native boolean getTopicPersistent(int topic);

  public static native void setTopicRetained(int topic, boolean value);

  public static native boolean getTopicRetained(int topic);

  public static native String getTopicTypeString(int topic);

  public static native boolean getTopicExists(int topic);

  public static native String getTopicProperty(int topic, String name);

  public static native void setTopicProperty(int topic, String name, String value);

  public static native void deleteTopicProperty(int topic, String name);

  public static native String getTopicProperties(int topic);

  public static native void setTopicProperties(int topic, String properties);

  public static native int subscribe(
      int topic, int type, String typeStr, PubSubOptions options);

  public static int subscribe(
      int topic, int type, String typeStr, PubSubOption... options) {
    return subscribe(topic, type, typeStr, buildOptions(options));
  }

  public static native void unsubscribe(int sub);

  public static native int publish(
      int topic, int type, String typeStr, PubSubOptions options);

  public static int publish(
      int topic, int type, String typeStr, PubSubOption... options) {
    return publish(topic, type, typeStr, buildOptions(options));
  }

  public static native int publishEx(
      int topic, int type, String typeStr, String properties, PubSubOptions options);

  public static int publishEx(
      int topic, int type, String typeStr, String properties, PubSubOption... options) {
    return publishEx(topic, type, typeStr, properties, buildOptions(options));
  }

  public static native void unpublish(int pubentry);

  public static native void releaseEntry(int entry);

  public static native void release(int pubsubentry);

  public static native int getTopicFromHandle(int pubsubentry);

  public static native int subscribeMultiple(int inst, String[] prefixes, PubSubOptions options);

  public static int subscribeMultiple(int inst, String[] prefixes, PubSubOption... options) {
    return subscribeMultiple(inst, prefixes, buildOptions(options));
  }

  public static native void unsubscribeMultiple(int sub);

  public static native TimestampedBoolean getAtomicBoolean(
      int subentry, boolean defaultValue);

  public static native TimestampedBoolean[] readQueueBoolean(int subentry);

  public static native boolean[] readQueueValuesBoolean(int subentry);

  public static native boolean setBoolean(int entry, long time, boolean value);

  public static native boolean getBoolean(int entry, boolean defaultValue);

  public static native boolean setDefaultBoolean(int entry, long time, boolean defaultValue);


  public static native TimestampedInteger getAtomicInteger(
      int subentry, long defaultValue);

  public static native TimestampedInteger[] readQueueInteger(int subentry);

  public static native long[] readQueueValuesInteger(int subentry);

  public static native boolean setInteger(int entry, long time, long value);

  public static native long getInteger(int entry, long defaultValue);

  public static native boolean setDefaultInteger(int entry, long time, long defaultValue);


  public static native TimestampedFloat getAtomicFloat(
      int subentry, float defaultValue);

  public static native TimestampedFloat[] readQueueFloat(int subentry);

  public static native float[] readQueueValuesFloat(int subentry);

  public static native boolean setFloat(int entry, long time, float value);

  public static native float getFloat(int entry, float defaultValue);

  public static native boolean setDefaultFloat(int entry, long time, float defaultValue);


  public static native TimestampedDouble getAtomicDouble(
      int subentry, double defaultValue);

  public static native TimestampedDouble[] readQueueDouble(int subentry);

  public static native double[] readQueueValuesDouble(int subentry);

  public static native boolean setDouble(int entry, long time, double value);

  public static native double getDouble(int entry, double defaultValue);

  public static native boolean setDefaultDouble(int entry, long time, double defaultValue);


  public static native TimestampedString getAtomicString(
      int subentry, String defaultValue);

  public static native TimestampedString[] readQueueString(int subentry);

  public static native String[] readQueueValuesString(int subentry);

  public static native boolean setString(int entry, long time, String value);

  public static native String getString(int entry, String defaultValue);

  public static native boolean setDefaultString(int entry, long time, String defaultValue);


  public static native TimestampedRaw getAtomicRaw(
      int subentry, byte[] defaultValue);

  public static native TimestampedRaw[] readQueueRaw(int subentry);

  public static native byte[][] readQueueValuesRaw(int subentry);

  public static boolean setRaw(int entry, long time, byte[] value) {
    return setRaw(entry, time, value, 0, value.length);
  }

  public static native boolean setRaw(int entry, long time, byte[] value, int start, int len);

  public static boolean setRaw(int entry, long time, ByteBuffer value) {
    int pos = value.position();
    return setRaw(entry, time, value, pos, value.capacity() - pos);
  }

  public static boolean setRaw(int entry, long time, ByteBuffer value, int start, int len) {
    if (value.isDirect()) {
      if (start < 0) {
        throw new IndexOutOfBoundsException("start must be >= 0");
      }
      if (len < 0) {
        throw new IndexOutOfBoundsException("len must be >= 0");
      }
      if ((start + len) > value.capacity()) {
        throw new IndexOutOfBoundsException("start + len must be smaller than buffer capacity");
      }
      return setRawBuffer(entry, time, value, start, len);
    } else if (value.hasArray()) {
      return setRaw(entry, time, value.array(), value.arrayOffset() + start, len);
    } else {
      throw new UnsupportedOperationException("ByteBuffer must be direct or have a backing array");
    }
  }

  private static native boolean setRawBuffer(int entry, long time, ByteBuffer value, int start, int len);

  public static native byte[] getRaw(int entry, byte[] defaultValue);

  public static boolean setDefaultRaw(int entry, long time, byte[] defaultValue) {
    return setDefaultRaw(entry, time, defaultValue, 0, defaultValue.length);
  }

  public static native boolean setDefaultRaw(int entry, long time, byte[] defaultValue, int start, int len);

  public static boolean setDefaultRaw(int entry, long time, ByteBuffer defaultValue) {
    int pos = defaultValue.position();
    return setDefaultRaw(entry, time, defaultValue, pos, defaultValue.limit() - pos);
  }

  public static boolean setDefaultRaw(int entry, long time, ByteBuffer defaultValue, int start, int len) {
    if (defaultValue.isDirect()) {
      if (start < 0) {
        throw new IndexOutOfBoundsException("start must be >= 0");
      }
      if (len < 0) {
        throw new IndexOutOfBoundsException("len must be >= 0");
      }
      if ((start + len) > defaultValue.capacity()) {
        throw new IndexOutOfBoundsException("start + len must be smaller than buffer capacity");
      }
      return setDefaultRawBuffer(entry, time, defaultValue, start, len);
    } else if (defaultValue.hasArray()) {
      return setDefaultRaw(entry, time, defaultValue.array(), defaultValue.arrayOffset() + start, len);
    } else {
      throw new UnsupportedOperationException("ByteBuffer must be direct or have a backing array");
    }
  }

  private static native boolean setDefaultRawBuffer(int entry, long time, ByteBuffer defaultValue, int start, int len);


  public static native TimestampedBooleanArray getAtomicBooleanArray(
      int subentry, boolean[] defaultValue);

  public static native TimestampedBooleanArray[] readQueueBooleanArray(int subentry);

  public static native boolean[][] readQueueValuesBooleanArray(int subentry);

  public static native boolean setBooleanArray(int entry, long time, boolean[] value);

  public static native boolean[] getBooleanArray(int entry, boolean[] defaultValue);

  public static native boolean setDefaultBooleanArray(int entry, long time, boolean[] defaultValue);


  public static native TimestampedIntegerArray getAtomicIntegerArray(
      int subentry, long[] defaultValue);

  public static native TimestampedIntegerArray[] readQueueIntegerArray(int subentry);

  public static native long[][] readQueueValuesIntegerArray(int subentry);

  public static native boolean setIntegerArray(int entry, long time, long[] value);

  public static native long[] getIntegerArray(int entry, long[] defaultValue);

  public static native boolean setDefaultIntegerArray(int entry, long time, long[] defaultValue);


  public static native TimestampedFloatArray getAtomicFloatArray(
      int subentry, float[] defaultValue);

  public static native TimestampedFloatArray[] readQueueFloatArray(int subentry);

  public static native float[][] readQueueValuesFloatArray(int subentry);

  public static native boolean setFloatArray(int entry, long time, float[] value);

  public static native float[] getFloatArray(int entry, float[] defaultValue);

  public static native boolean setDefaultFloatArray(int entry, long time, float[] defaultValue);


  public static native TimestampedDoubleArray getAtomicDoubleArray(
      int subentry, double[] defaultValue);

  public static native TimestampedDoubleArray[] readQueueDoubleArray(int subentry);

  public static native double[][] readQueueValuesDoubleArray(int subentry);

  public static native boolean setDoubleArray(int entry, long time, double[] value);

  public static native double[] getDoubleArray(int entry, double[] defaultValue);

  public static native boolean setDefaultDoubleArray(int entry, long time, double[] defaultValue);


  public static native TimestampedStringArray getAtomicStringArray(
      int subentry, String[] defaultValue);

  public static native TimestampedStringArray[] readQueueStringArray(int subentry);

  public static native String[][] readQueueValuesStringArray(int subentry);

  public static native boolean setStringArray(int entry, long time, String[] value);

  public static native String[] getStringArray(int entry, String[] defaultValue);

  public static native boolean setDefaultStringArray(int entry, long time, String[] defaultValue);


  public static native NetworkTableValue[] readQueueValue(int subentry);

  public static native NetworkTableValue getValue(int entry);

  public static native void setEntryFlags(int entry, int flags);

  public static native int getEntryFlags(int entry);

  public static native TopicInfo getTopicInfo(NetworkTableInstance inst, int topic);

  public static native int createListenerPoller(int inst);

  public static native void destroyListenerPoller(int poller);

  private static int kindsToMask(EnumSet<NetworkTableEvent.Kind> kinds) {
    int mask = 0;
    for (NetworkTableEvent.Kind kind : kinds) {
      mask |= kind.getValue();
    }
    return mask;
  }

  public static int addListener(int poller, String[] prefixes, EnumSet<NetworkTableEvent.Kind> kinds) {
    return addListener(poller, prefixes, kindsToMask(kinds));
  }

  public static int addListener(int poller, int handle, EnumSet<NetworkTableEvent.Kind> kinds) {
    return addListener(poller, handle, kindsToMask(kinds));
  }

  public static native int addListener(int poller, String[] prefixes, int mask);

  public static native int addListener(int poller, int handle, int mask);

  public static native NetworkTableEvent[] readListenerQueue(
      NetworkTableInstance inst, int poller);

  public static native void removeListener(int listener);

  public static native int getNetworkMode(int inst);

  public static native void startLocal(int inst);

  public static native void stopLocal(int inst);

  public static native void startServer(
      int inst, String persistFilename, String listenAddress, int port3, int port4);

  public static native void stopServer(int inst);

  public static native void startClient3(int inst, String identity);

  public static native void startClient4(int inst, String identity);

  public static native void stopClient(int inst);

  public static native void setServer(int inst, String serverName, int port);

  public static native void setServer(int inst, String[] serverNames, int[] ports);

  public static native void setServerTeam(int inst, int team, int port);

  public static native void disconnect(int inst);

  public static native void startDSClient(int inst, int port);

  public static native void stopDSClient(int inst);

  public static native void flushLocal(int inst);

  public static native void flush(int inst);

  public static native ConnectionInfo[] getConnections(int inst);

  public static native boolean isConnected(int inst);

  public static native OptionalLong getServerTimeOffset(int inst);

  public static native long now();

  private static native int startEntryDataLog(int inst, long log, String prefix, String logPrefix);

  public static int startEntryDataLog(int inst, DataLog log, String prefix, String logPrefix) {
    return startEntryDataLog(inst, log.getImpl(), prefix, logPrefix);
  }

  public static native void stopEntryDataLog(int logger);

  private static native int startConnectionDataLog(int inst, long log, String name);

  public static int startConnectionDataLog(int inst, DataLog log, String name) {
    return startConnectionDataLog(inst, log.getImpl(), name);
  }

  public static native void stopConnectionDataLog(int logger);

  public static native int addLogger(int poller, int minLevel, int maxLevel);
}
