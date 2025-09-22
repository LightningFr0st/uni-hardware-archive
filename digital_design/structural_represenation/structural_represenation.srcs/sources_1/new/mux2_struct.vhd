----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/21/2025 10:12:46 PM
-- Design Name: 
-- Module Name: mux2_struct - Structural
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity mux2_struct is
    Port ( A : in STD_LOGIC;
           B : in STD_LOGIC;
           S : in STD_LOGIC;
           Q : out STD_LOGIC);
end mux2_struct;

architecture Structural of mux2_struct is

component inv is
    port(I : in STD_LOGIC; O : out STD_LOGIC);
end component; 

component and2 is
    Port ( A : in STD_LOGIC; B : in STD_LOGIC; Q : out STD_LOGIC);
end component;

component or2 is
    Port ( A : in STD_LOGIC; B : in STD_LOGIC; Q : out STD_LOGIC);
end component;

signal SN, ASN, SB : std_logic;

begin
    U1: inv port map (I => S, O => SN); 
    U2: and2 port map (A => A, B => SN, Q => ASN);
    U3: and2 port map (A => B, B => S, Q => SB); 
    U4: or2 port map (A => ASN, B => SB, Q => Q);
end Structural;
