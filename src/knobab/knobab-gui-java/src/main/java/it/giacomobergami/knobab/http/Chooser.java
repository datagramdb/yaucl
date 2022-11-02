package it.giacomobergami.knobab.http;

import javax.swing.*;
import java.util.*;

public class Chooser<T> {

    HashMap<String, T> elements;
    String[] choices;
    String title;
    String message;

    public Chooser(Set<T> elements, String title, String message) {
        this.elements = new HashMap<>();
        this.title = title;
        this.message = message;
        elements.forEach(x -> this.elements.put(x.toString(), x));
        choices = elements.stream().map(Object::toString).toArray(value -> new String[elements.size()]);
    }

    public static Chooser<String> stringOptions(String[] elements, String title, String message) {
        if (elements == null || elements.length == 0)
            return new Chooser<>(new HashSet<>(), title, message);
        else
            return new Chooser<>(new HashSet<>(Arrays.asList(elements)), title, message);
    }

    public T run() {
        if (elements.isEmpty()) {
            JOptionPane.showMessageDialog(null, "No Option Available", title, JOptionPane.INFORMATION_MESSAGE);
            return null;
        }
        return elements.get((String)JOptionPane.showInputDialog(null, message,
                title, JOptionPane.QUESTION_MESSAGE, null, // Use
                // default
                // icon
                choices, // Array of choices
                choices[0])); // Initial choice
    }

}
