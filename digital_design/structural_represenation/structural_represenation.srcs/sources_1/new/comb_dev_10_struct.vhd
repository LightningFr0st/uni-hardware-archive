----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/21/2025 10:19:37 PM
-- Design Name: 
-- Module Name: comb_dev_10_struct - Behavioral
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

entity comb_dev_10_struct is
    Port ( X : in STD_LOGIC;
           Y : in STD_LOGIC;
           Z : in STD_LOGIC;
           F : out STD_LOGIC);
end comb_dev_10_struct;

architecture Structural of comb_dev_10_struct is

component inv is
    Port ( I : in STD_LOGIC; O : out STD_LOGIC );
end component;

component and2 is
    Port ( A : in STD_LOGIC; B : in STD_LOGIC; Q : out STD_LOGIC );
end component;

component and3 is
    Port ( A : in STD_LOGIC; B : in STD_LOGIC; C : in STD_LOGIC; Q : out STD_LOGIC );
end component;

component or3 is
    Port ( A : in STD_LOGIC; B : in STD_LOGIC; C : in STD_LOGIC; Q : out STD_LOGIC );
end component;

signal NY, NX, NZ : STD_LOGIC;
signal T1, T2, T3 : STD_LOGIC;

begin
    U1: inv port map (I => Y, O => NY);
    U2: inv port map (I => X, O => NX);
    U3: inv port map (I => Z, O => NZ);

    U4: and2 port map (A => X,  B => Z,  Q => T1);          -- X·Z
    U5: and2 port map (A => NY, B => Z,  Q => T2);          -- Y'·Z
    U6: and3 port map (A => NX, B => Y, C => NZ, Q => T3);  -- X'·Y·Z'

    U7: or3 port map (A => T1, B => T2, C => T3, Q => F);
end Structural;
