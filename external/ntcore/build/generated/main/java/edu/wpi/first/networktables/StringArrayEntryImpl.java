// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** NetworkTables StringArray implementation. */
@SuppressWarnings("PMD.ArrayIsStoredDirectly")
final class StringArrayEntryImpl extends EntryBase implements StringArrayEntry {
  /**
   * Constructor.
   *
   * @param topic Topic
   * @param handle Native handle
   * @param defaultValue Default value for get()
   */
  StringArrayEntryImpl(StringArrayTopic topic, int handle, String[] defaultValue) {
    super(handle);
    m_topic = topic;
    m_defaultValue = defaultValue;
  }

  @Override
  public StringArrayTopic getTopic() {
    return m_topic;
  }

  @Override
  public String[] get() {
    return NetworkTablesJNI.getStringArray(m_handle, m_defaultValue);
  }

  @Override
  public String[] get(String[] defaultValue) {
    return NetworkTablesJNI.getStringArray(m_handle, defaultValue);
  }

  @Override
  public TimestampedStringArray getAtomic() {
    return NetworkTablesJNI.getAtomicStringArray(m_handle, m_defaultValue);
  }

  @Override
  public TimestampedStringArray getAtomic(String[] defaultValue) {
    return NetworkTablesJNI.getAtomicStringArray(m_handle, defaultValue);
  }

  @Override
  public TimestampedStringArray[] readQueue() {
    return NetworkTablesJNI.readQueueStringArray(m_handle);
  }

  @Override
  public String[][] readQueueValues() {
    return NetworkTablesJNI.readQueueValuesStringArray(m_handle);
  }

  @Override
  public void set(String[] value, long time) {
    NetworkTablesJNI.setStringArray(m_handle, time, value);
  }

  @Override
  public void setDefault(String[] value) {
    NetworkTablesJNI.setDefaultStringArray(m_handle, 0, value);
  }

  @Override
  public void unpublish() {
    NetworkTablesJNI.unpublish(m_handle);
  }

  private final StringArrayTopic m_topic;
  private final String[] m_defaultValue;
}